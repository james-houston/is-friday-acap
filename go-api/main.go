package main

import (
	"math/rand"
	"net/http"

	"github.com/gin-gonic/gin"
)

const (
	localIP = "192.168.1.197:8080" // update with your IP before running
)

// isFridayResponse represents the struct used to hold the response to the /isfriday route
type isFridayResponse struct {
	Friday bool `json:"friday"`
}

func main() {
	router := gin.Default()
	router.GET("/isfriday", isFriday)

	router.Run(localIP)
}

// getAlbums responds with the list of all albums as JSON.
func isFriday(c *gin.Context) {

	/* TODO: This commented out block will return if true if the day of the week is Friday
	resp := isFridayResponse{
		Friday: false,
	}
	weekday := time.Now().Weekday()
	if weekday == time.Friday {
		resp.Friday = true
	}
	*/

	// For development and testing purposes, instead we randomly return true with a 1 in 5 chance
	randomChance := rand.Intn(5)
	resp := isFridayResponse{
		Friday: false,
	}
	if randomChance == 3 {
		resp.Friday = true
	}

	c.IndentedJSON(http.StatusOK, resp)
}
