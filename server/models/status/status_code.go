package status

type MyError struct {
	Code    StatusCode
	Message string
	Err     error
}

type StatusCode string

const (
	// business
	Success             StatusCode = "success"
	InsufficientBalance StatusCode = "not_enough_money"
	NoSubscription      StatusCode = "no_subscription"
	AlreadySubscribe    StatusCode = "active_subscription"
	// errors
	Err        StatusCode = "error"
	ErrJWT     StatusCode = "invalid_token"
	ErrReqBody StatusCode = "bad_request_format"
	ErrNoPlan  StatusCode = "plan_not_found"
	ErrNoQuest StatusCode = "quest_not_found"
)

func (err MyError) Error() string {
	return err.Message
}

func New(code StatusCode, text string, err error) *MyError {
	return &MyError{
		Code:    code,
		Message: text,
		Err:     err,
	}
}
