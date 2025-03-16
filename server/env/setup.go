package env

import (
	"fmt"
	"log/slog"
	"strings"

	"github.com/spf13/viper"
)

func SetupConfig() {
	slog.Info("Loading config ...")
	// Establishing Defaults
	// viper.SetDefault("ContentDir", "content")
	// viper.SetDefault("LayoutDir", "layouts")

	// Reading Config Files
	viper.AddConfigPath(".")
	viper.SetConfigName("config")
	viper.SetConfigType("yaml")

	viper.AutomaticEnv()
	viper.SetEnvKeyReplacer(strings.NewReplacer(".", "_"))
	if err := viper.ReadInConfig(); err != nil {
		panic(fmt.Errorf("fatal error reading config file: %w", err))
	}

	slog.Info("Starting app", "name", Get("app.name"))
}

func Get(key string) string {
	return viper.GetString(key)
}
