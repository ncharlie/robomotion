package dto

import "time"

type Robot struct {
	RobotId   string     `json:"r" validate:"required"`
	X         int        `json:"x" validate:"required"`
	Y         int        `json:"y" validate:"required"`
	Z         int        `json:"z" validate:"required"`
	Heading   int        `json:"h" validate:"required"`
	Speed     int        `json:"s" validate:"required"`
	TimeStamp *time.Time `json:"t,omitempty"`
}
