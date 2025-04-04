package entities

import (
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type Notification struct {
	ID        primitive.ObjectID `json:"-" bson:"_id,omitempty"`
	RobotId   string             `json:"r" bson:"robot_id"`
	CreatedAt time.Time          `json:"-" bson:"created_at"`
	Type      notiType           `json:"-" bson:"type"`
	Location  Location           `bson:"inline"`
}

type notiType string

const (
	NotiObstacle notiType = "obstacle"
	NotiZone     notiType = "zone"
)
