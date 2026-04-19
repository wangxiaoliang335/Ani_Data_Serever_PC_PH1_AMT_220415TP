#pragma once

// 点灯检事件回调接口
// 由需要接收 Lighting 消息的类实现
class ILightingEventHandler
{
public:
	virtual ~ILightingEventHandler() {}

	// Lighting -> MC : Running@
	// 通知"点灯检测已经开始 Running"
	virtual void OnLightingRunning() {}

	// Lighting -> MC : SnapFN@
	// 通知"点灯采图完成，可以出栈/移动工位"
	virtual void OnLightingSnapFN() {}

	// Lighting -> MC : FN$xxxxxxxx@
	// payload 已经被解析成 4 个治具结果码（resultCode[0..3]），空位为 0
	virtual void OnLightingResult(const int resultCode[4]) {}

	// 连接成功后定时发送测试信号
	virtual void OnLightingAutoTest() {}
};
