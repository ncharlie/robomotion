package entities

import (
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type TransactionLog struct {
	ID        primitive.ObjectID `json:"-" bson:"_id,omitempty"`
	Type      logType            `json:"-" bson:"type"`
	RobotID   string             `json:"-" bson:"robot_id"`
	Remarks   string             `json:"-" bson:"remarks"`
	CreatedAt time.Time          `json:"-" bson:"created_at"`
}

type logType string

const (
	LogLocation logType = "location"
	LogObstacle logType = "obstacle"
	LogMovement logType = "movement"
)
