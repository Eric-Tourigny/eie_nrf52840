import pygame
import logging

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(name=__name__)


def init_controller():
    logger.debug("Searching for controllers")
    pygame.init()

    while True:
        pygame.joystick.init()                              # Detect controllers

        if pygame.joystick.get_count() != 0:                # If any controllers are found
            controller = pygame.joystick.Joystick(0)        # Take the first controller
            controller.init()                               # Initialize it
            logger.debug("Controller Connected")
            return controller
        
        pygame.joystick.quit()                              # Clear controller data, so we can try again
    
def main():
    controller = init_controller()


if __name__ == "__main__":
    main()