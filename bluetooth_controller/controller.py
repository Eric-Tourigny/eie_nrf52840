import pygame

button_names = ["A", "B", "X", "Y"]

def run(controller: pygame.joystick.JoystickType):
    while True:
        for event in pygame.event.get():
            match event.type:
                case pygame.JOYAXISMOTION:
                    pass
                case pygame.JOYBUTTONDOWN:
                    print(f"Button down - {event.dict["button"]}")
                    #if controller.rumble(1.0, 1.0, 1000):
                    #    print("Rumble played")
                    #else:
                    #    print("Rumble failed")
                case pygame.JOYBUTTONUP:
                    print(f"Button up - {event.dict["button"]}")
                case _:
                    print(f"Unrecognized event: {event}")
def main():
    pygame.init()
    pygame.joystick.init()

    if pygame.joystick.get_count() == 0:
        print("No controller detected")
    else:
        controller = pygame.joystick.Joystick(0)
        controller.init()
        run(controller)

if __name__ == "__main__":
    main()