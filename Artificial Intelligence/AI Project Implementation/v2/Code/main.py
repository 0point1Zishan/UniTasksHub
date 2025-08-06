import pygame
import sys
from constants import *
from game import Game

def get_row_col_from_mouse(pos):
    x, y = pos
    if x >= HEIGHT: # Mouse is outside the board area
        return -1, -1
    row = y // SQ_SIZE
    col = x // SQ_SIZE
    return row, col

def main():
    pygame.init()
    win = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption('Chess AI Agent')
    clock = pygame.time.Clock()
    game = Game(win)

    running = True
    while running:
        clock.tick(60)

        # AI's turn to move
        if game.turn == 'b' and not game.game_over:
            game.ai_move()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if event.type == pygame.MOUSEBUTTONDOWN:
                if game.turn == 'w': # Human player's turn
                    pos = pygame.mouse.get_pos()
                    row, col = get_row_col_from_mouse(pos)
                    if row != -1:
                        game.select(row, col)

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_r:  # Reset the game
                    game = Game(win)

        game.update()

    pygame.quit()
    sys.exit()

if __name__ == '__main__':
    main()