package main

import (
	"os"
	"time"
)

const (
	Location_default string = "UTC"
	Location_env     string = "AECDFT_TZ"
)

func SetLocation() (*time.Location, error) {
	location_from_env := os.Getenv(Location_env)
	if location_from_env == "" {
		location_from_env = Location_default
	}
	return time.LoadLocation(location_from_env)
}
