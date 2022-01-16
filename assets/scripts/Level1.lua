Level1 = {
    assets = {
        [0] = { type="texture", id = "terrain-texture-day", file = "./assets/tilemaps/jungle.png" },
        [1] = { type="texture", id = "terrain-texture-night", file = "./assets/tilemaps/jungle-night.png" },
        [2] = { type="texture", id = "chopper-texture", file = "./assets/images/chopper-spritesheet.png" },
        [3] = { type="texture", id = "tank-texture-big-left", file = "./assets/images/tank-big-left.png" },
        [4] = { type="texture", id = "projectile-texture", file = "./assets/images/bullet-enemy.png" },
        [5] = { type="font", id = "charriot-font", file = "./assets/fonts/charriot.ttf", fontSize = 24 },
        [6] = { type="texture", id = "radar-image", file = "./assets/images/radar.png" },
        [7] = { type="texture", id = "tank-texture-big-right", file = "./assets/images/tank-big-right.png" }
    },
    map = {
        textureAssetId = "terrain-texture-day",
        file = "./assets/tilemaps/jungle.map",
        scale = 3,
        tileSize = 32,
        size = {
            x = 25,
            y = 20
        }
    },
    entities = {
        [0] = {
            name = "player",
            layer = 4,
            components = {
                transform = {
                    position = {
                        x = 240,
                        y = 106
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 32,
                    height = 32,
                    scale = 3,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "chopper-texture",
                    animated = true,
                    fixed = false,
                    _animated = {
                        frameCount = 2,
                        animationSpeed = 90,
                        hasDirections = true,
                    }
                },
                collider = {
                    tag = "PLAYER"
                },
                input = {
                    keyboard = {
                        up = "w",
                        left = "a",
                        down = "s",
                        right = "d",
                        shoot = "space"
                    }
                }
            }
        },
        [1] = {
            name = "tank-enemy-1",
            layer = 2,
            components = {
                transform = {
                    position = {
                        x = 300,
                        y = 730
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 32,
                    height = 32,
                    scale = 2,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "tank-texture-big-right",
                    animated = false,
                    fixed = false
                },
                collider = {
                    tag = "ENEMY"
                },
                projectileEmitter = {
                    name = "projectile-tank-enemy-1",
                    speed = 250,
                    range = 800,
                    angle = 340,
                    width = 4,
                    height = 4,
                    shouldLoop = true,
                    textureAssetId = "projectile-texture",
                    collider = {
                        tag = "PROJECTILE"
                    }
                }
            }
        },
        [2] = {
            name = "radar-ui",
            layer = 5,
            components = {
                transform = {
                    position = {
                        x = 720,
                        y = 15
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 64,
                    height = 64,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "radar-image",
                    animated = true,
                    fixed = true,
                    _animated = {
                        frameCount = 8,
                        animationSpeed = 150,
                        hasDirections = false
                    }
                }
            }
        },
        [3] = {
            name = "level-text",
            layer = 5,
            components = {
                label = {
                    font = "charriot-font",
                    text = "First Level...",
                    position = {
                        x = 10,
                        y = 10
                    }
                }
            }
            
        }
    }
}
