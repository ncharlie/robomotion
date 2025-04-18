package entities

import (
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type Robot struct {
	ID        primitive.ObjectID `json:"-" bson:"_id,omitempty"`
	RobotId   string             `json:"r" bson:"robot_id"`
	Status    robotStatus        `json:"-" bson:"status"`
	CreatedAt time.Time          `json:"-" bson:"created_at"`
	UpdatedAt time.Time          `json:"-" bson:"updated_at"`
}

type robotStatus string

const (
	RobotOnline  robotStatus = "online"
	RobotOffline robotStatus = "offline"
)
