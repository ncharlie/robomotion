package entities

import (
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type Beacon struct {
	ID        primitive.ObjectID `bson:"_id,omitempty"`
	BeaconId  string             `bson:"beacon_id"`
	CreatedAt time.Time          `bson:"created_at"`
	UpdatedAt time.Time          `bson:"updated_at"`
	Location  Location           `bson:"inline"`
}
