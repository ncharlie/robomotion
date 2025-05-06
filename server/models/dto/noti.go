package dto

import "time"

type Notification struct {
	RobotId   string    `json:"r" validate:"required"`
	Type      string    `json:"t" validate:"required"`
	TimeStamp time.Time `json:"ts"`
}
