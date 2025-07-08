#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

typedef enum {
  KEY_ACTION_NONE = 0,
  KEY_ACTION_SINGLE_CLICK, // 单击
  KEY_ACTION_DOUBLE_CLICK, // 双击
  KEY_ACTION_LONG_PRESS, // 长按
	KEY_ACTION_LONG_PRESS_REPEAT,
} key_action_t;

#endif