package dto

type Notification struct {
	RobotId string `json:"r" validate:"required"`
	Type    string `json:"t" validate:"required"`
}
