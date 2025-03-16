package utils

import (
	"github.com/lestrrat-go/jwx/v2/jwa"
	"github.com/lestrrat-go/jwx/v2/jwt"
)

func VerifyJWT(jwtSecretKey []byte, sessionToken string) (jwt.Token, error) {
	return jwt.ParseString(sessionToken, jwt.WithKey(jwa.HS256, jwtSecretKey))
}

// func VerifyJWT(jwtSecretKey string, sessionToken string) string {

// 	parsed, err := jwt.Parse(sessionToken, func(token *jwt.Token) (interface{}, error) {
// 		if _, ok := token.Method.(*jwt.SigningMethodHMAC); !ok {
// 			return nil, errors.New("unexpected signing method")
// 		}

// 		return []byte(jwtSecretKey), nil
// 	})

// 	fmt.Println(parsed.Valid)
// 	for key, val := range parsed.Claims {
// 		fmt.Printf("Key: %v, value: %v\n", key, val)
// 	}
// 	fmt.Println(err)
// 	return ""
// }
