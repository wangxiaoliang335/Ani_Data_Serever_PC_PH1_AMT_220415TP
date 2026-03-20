# -*- coding: utf-8 -*-
"""
点灯检服务器模拟程序
监听 6501 端口，模拟 AOI/点灯检测设备的 TCP 通信
"""

import socket
import struct
import threading
import time
import random
import json
from datetime import datetime

# 配置
SERVER_HOST = '0.0.0.0'
SERVER_PORT = 6501

class LightingSimulator:
    def __init__(self):
        self.running = False
        self.clients = []
        self.guardian_counter = 0
        
    def log(self, msg):
        """带时间戳的日志输出"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        print(f"[{timestamp}] {msg}")
        
    def generate_barcode(self):
        """生成模拟条码"""
        prefix = "CSOT"
        number = random.randint(10000000, 99999999)
        return f"{prefix}{number}"
    
    def generate_defect_response(self, barcode):
        """
        生成缺陷响应数据
        根据实际协议格式生成
        """
        # 随机生成缺陷数量 0-3
        defect_count = random.randint(0, 3)
        
        defects = []
        for i in range(defect_count):
            # 模拟缺陷代码和等级
            code_options = ["P1", "P2", "L1", "L2", "M1", "D1", "B1"]
            grade_options = ["A", "B", "C", "R1", "R2"]
            
            defect = {
                "DefectIndex": i + 1,
                "Type": random.choice(["POINT", "LINE", "MURA", "BUBBLE", "BAND"]),
                "Code": random.choice(code_options),
                "Grade": random.choice(grade_options),
                "PosX": random.randint(100, 1900),
                "PosY": random.randint(100, 1100),
                "Size": random.randint(5, 50)
            }
            defects.append(defect)
        
        return {
            "Barcode": barcode,
            "Result": "NG" if defect_count > 0 else "OK",
            "DefectCount": defect_count,
            "Defects": defects,
            "Timestamp": datetime.now().strftime("%Y%m%d%H%M%S")
        }
    
    def parse_request(self, data):
        """解析客户端请求"""
        try:
            # 尝试解析为 JSON
            return json.loads(data.decode('utf-8'))
        except:
            # 如果不是 JSON，尝试其他格式
            return {"RawData": data.decode('utf-8', errors='ignore').strip()}
    
    def create_response(self, request):
        """根据请求创建响应"""
        req_type = request.get("Type", "")
        
        if req_type == "QUERY_BARCODE":
            # 查询条码状态
            barcode = request.get("Barcode", self.generate_barcode())
            return self.generate_defect_response(barcode)
            
        elif req_type == "HEARTBEAT":
            # 心跳检测
            return {"Type": "HEARTBEAT_ACK", "Status": "OK", "Timestamp": time.time()}
            
        elif req_type == "UPLOAD_RESULT":
            # 上传结果确认
            barcode = request.get("Barcode", "")
            return {
                "Type": "UPLOAD_ACK", 
                "Barcode": barcode, 
                "Status": "OK",
                "Timestamp": time.time()
            }
        else:
            # 默认响应
            return {"Status": "UNKNOWN_REQUEST", "Received": request}
    
    def handle_client(self, client_socket, address):
        """处理客户端连接"""
        self.log(f"客户端连接: {address}")
        self.clients.append(client_socket)
        
        try:
            while self.running:
                # 设置超时，以便定期检查 running 状态
                client_socket.settimeout(1.0)
                
                try:
                    # 接收数据
                    data = client_socket.recv(4096)
                    
                    if not data:
                        self.log(f"客户端断开: {address}")
                        break
                    
                    self.log(f"收到数据 from {address}: {data[:100]}...")
                    
                    # 解析请求
                    request = self.parse_request(data)
                    
                    # 生成响应
                    response = self.create_response(request)
                    
                    # 发送响应
                    response_data = json.dumps(response).encode('utf-8')
                    client_socket.sendall(response_data)
                    self.log(f"发送响应 to {address}: {response_data[:100]}...")
                    
                except socket.timeout:
                    # 超时，继续循环检查 running 状态
                    continue
                    
        except Exception as e:
            self.log(f"客户端异常 {address}: {e}")
            
        finally:
            try:
                client_socket.close()
            except:
                pass
            if client_socket in self.clients:
                self.clients.remove(client_socket)
            self.log(f"连接已关闭: {address}")
    
    def send_guardian(self):
        """发送守护消息"""
        self.guardian_counter += 1
        guardian_msg = f"GUARDIAN|{self.guardian_counter}|{datetime.now().strftime('%Y%m%d%H%M%S')}"
        self.log(f"守护消息: {guardian_msg}")
        
        # 可以广播给所有客户端（如果协议需要）
        # for client in self.clients:
        #     try:
        #         client.sendall(guardian_msg.encode('utf-8'))
        #     except:
        #         pass
    
    def start(self):
        """启动服务器"""
        self.running = True
        
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        try:
            server_socket.bind((SERVER_HOST, SERVER_PORT))
            server_socket.listen(5)
            
            self.log(f"=" * 50)
            self.log(f"点灯检模拟服务器已启动")
            self.log(f"监听地址: {SERVER_HOST}:{SERVER_PORT}")
            self.log("=" * 50)
            
            # 启动守护消息线程
            guardian_thread = threading.Thread(target=self._guardian_loop, daemon=True)
            guardian_thread.start()
            
            # 启动心跳检测线程
            heartbeat_thread = threading.Thread(target=self._heartbeat_loop, daemon=True)
            heartbeat_thread.start()
            
            while self.running:
                try:
                    server_socket.settimeout(1.0)
                    client_socket, address = server_socket.accept()
                    
                    # 为每个客户端创建处理线程
                    client_thread = threading.Thread(
                        target=self.handle_client, 
                        args=(client_socket, address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.timeout:
                    continue
                    
        except OSError as e:
            self.log(f"端口绑定失败: {e}")
            self.log(f"端口 {SERVER_PORT} 可能已被占用，请先关闭占用程序")
            
        except Exception as e:
            self.log(f"服务器异常: {e}")
            
        finally:
            self.stop()
    
    def _guardian_loop(self):
        """守护消息循环"""
        while self.running:
            time.sleep(10)  # 每10秒发送一次
            if self.running:
                self.send_guardian()
    
    def _heartbeat_loop(self):
        """心跳检测循环"""
        while self.running:
            time.sleep(5)  # 每5秒检测一次
            if self.running:
                # 检查客户端连接状态
                dead_clients = []
                for client in self.clients:
                    try:
                        # 发送空数据检测连接
                        client.sendall(b"")
                    except:
                        dead_clients.append(client)
                
                for client in dead_clients:
                    try:
                        client.close()
                    except:
                        pass
                    if client in self.clients:
                        self.clients.remove(client)
    
    def stop(self):
        """停止服务器"""
        self.log("正在停止服务器...")
        self.running = False
        
        # 关闭所有客户端连接
        for client in self.clients:
            try:
                client.close()
            except:
                pass
        self.clients.clear()
        
        self.log("服务器已停止")


def main():
    print("=" * 60)
    print("    点灯检服务器模拟程序 v1.0")
    print("=" * 60)
    print()
    print("功能说明:")
    print("  - 监听端口 6501")
    print("  - 接收并响应点灯检设备的 TCP 请求")
    print("  - 自动生成模拟缺陷数据")
    print()
    print("按 Ctrl+C 停止服务器")
    print("=" * 60)
    print()
    
    simulator = LightingSimulator()
    
    try:
        simulator.start()
    except KeyboardInterrupt:
        print("\n收到停止信号...")
        simulator.stop()
        print("程序已退出")


if __name__ == "__main__":
    main()
