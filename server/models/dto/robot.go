package dto

import "time"

type Robot struct {
	RobotId   string     `json:"r" validate:"required"`
	X         int        `json:"x" validate:"required"`
	Y         int        `json:"y" validate:"required"`
	Z         int        `json:"z"`
	Heading   int        `json:"h"`
	Speed     int        `json:"s"`
	TimeStamp *time.Time `json:"t,omitempty"`
}
