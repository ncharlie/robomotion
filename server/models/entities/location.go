package entities

type Location struct {
	X       int `json:"x" bson:"x"`
	Y       int `json:"y" bson:"y"`
	Z       int `json:"z" bson:"z"`
	Heading int `json:"heading" bson:"heading"`
	Speed   int `json:"speed" bson:"speed"`
}
