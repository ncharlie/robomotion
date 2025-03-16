# build stage ----------------------------------------------------
FROM golang:1.24.1-alpine3.21 as builder

# prepare dependencies ----------------------------------------------------
RUN apk update && apk add --no-cache git ca-certificates tzdata && update-ca-certificates

WORKDIR /build
COPY go.mod go.sum ./
RUN go mod download && go mod verify

# build ----------------------------------------------------
COPY . ./
RUN GOOS=linux GOARCH=amd64 go build -ldflags="-w -s" -o /go/bin/robomotion


# run stage ----------------------------------------------------

FROM scratch

# -----------------------------------------------------------------
WORKDIR /app
COPY --from=builder /etc/ssl/cert.pem /etc/ssl/cert.pem
COPY --from=builder /usr/share/zoneinfo /usr/share/zoneinfo
COPY --from=builder /go/bin/robomotion ./
COPY config.yaml ./

# set timezone ----------------------------------------------------
ARG TZ='Asia/Bangkok'

EXPOSE 3000
EXPOSE 3001/udp
ENTRYPOINT ["./robomotion"]

# CMD ["test"]