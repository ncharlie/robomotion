package entities

import (
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type Robot struct {
	ID        primitive.ObjectID `json:"-" bson:"_id,omitempty"`
	Type      logType            `json:"-" bson:"type"`
	RobotID   string             `json:"-" bson:"robot_id"`
	Remarks   string             `json:"-" bson:"remarks"`
	CreatedAt time.Time          `json:"-" bson:"created_at"`
	RobotId   string             `json:"r" validate:"required"`
	X         int                `json:"x" validate:"required"`
	Y         int                `json:"y" validate:"required"`
	Z         int                `json:"z" validate:"required"`
	Heading   int                `json:"h" validate:"required"`
	Speed     int                `json:"s" validate:"required"`
}
