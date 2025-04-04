package entities

import (
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type Update struct {
	ID        primitive.ObjectID `json:"-" bson:"_id,omitempty"`
	RobotId   string             `json:"r" bson:"robot_id"`
	CreatedAt time.Time          `json:"-" bson:"created_at"`
	Location  Location           `bson:"inline"`
}
