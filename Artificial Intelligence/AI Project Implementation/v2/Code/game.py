import pygame
from constants import *
from board import Board
from ai import ChessAI

class Game:
    def __init__(self, win):
        self.win = win
        self.board = Board()
        self.turn = 'w'  # White starts
        self.selected_piece = None
        self.valid_moves = []
        self.ai = ChessAI(depth=3)
        self.game_over = False
        self.winner = None

    def update(self):
        self.board.draw(self.win)
        self.draw_valid_moves()
        self.draw_game_info()
        pygame.display.update()

    def draw_game_info(self):
        # Create a surface for the info panel
        info_panel = pygame.Surface((WIDTH - HEIGHT, HEIGHT))
        info_panel.fill(INFO_BG)

        font = pygame.font.Font(None, 30)
        y_pos = 20

        # Turn indicator
        if not self.game_over:
            color_name = "White" if self.turn == 'w' else "Black"
            text = font.render(f"{color_name}'s Turn", True, BLACK)
            info_panel.blit(text, (10, y_pos))
            y_pos += 40
        else:
            if self.winner:
                win_text = f"{self.winner} wins by Checkmate!"
            else:
                win_text = "Draw by Stalemate!"
            text = font.render(win_text, True, RED)
            info_panel.blit(text, (10, y_pos))
            y_pos += 40
        
        # Reset instructions
        reset_font = pygame.font.Font(None, 24)
        reset_text = reset_font.render("Press 'R' to Reset", True, BLACK)
        info_panel.blit(reset_text, (10, HEIGHT - 40))

        self.win.blit(info_panel, (HEIGHT, 0))


    def select(self, row, col):
        if self.game_over:
            return

        if self.selected_piece:
            # Try to move to the selected square
            result = self._move(row, col)
            # If the move was not valid, deselect and try to select a new piece
            if not result:
                self.selected_piece = None
                self.valid_moves = []
                self.select(row, col)
        
        piece = self.board.get_piece(row, col)
        if piece and piece.color == self.turn:
            self.selected_piece = piece
            self.valid_moves = self.board.get_valid_moves(piece)
            return True
        return False

    def _move(self, row, col):
        if self.selected_piece and (row, col) in self.valid_moves:
            self.board.move_piece(self.selected_piece, row, col)
            self.selected_piece = None
            self.valid_moves = []
            self.change_turn()
            return True
        return False

    def draw_valid_moves(self):
        for move in self.valid_moves:
            row, col = move
            surface = pygame.Surface((SQ_SIZE, SQ_SIZE), pygame.SRCALPHA)
            pygame.draw.circle(surface, HIGHLIGHT_COLOR, (SQ_SIZE//2, SQ_SIZE//2), SQ_SIZE//6)
            self.win.blit(surface, (col * SQ_SIZE, row * SQ_SIZE))

    def change_turn(self):
        self.turn = 'b' if self.turn == 'w' else 'w'
        self.check_game_over()

    def check_game_over(self):
        if self.board.is_checkmate(self.turn):
            self.game_over = True
            self.winner = "Black" if self.turn == 'w' else "White"
        elif self.board.is_stalemate(self.turn):
            self.game_over = True
            self.winner = None

    def ai_move(self):
        if self.game_over or self.turn != 'b':
            return
        
        # Display AI thinking status
        pygame.display.set_caption('Chess AI Agent - AI is thinking...')
        self.update()

        best_move = self.ai.get_best_move(self.board, 'b')
        
        pygame.display.set_caption('Chess AI Agent')

        if best_move:
            piece, (new_row, new_col) = best_move
            self.board.move_piece(piece, new_row, new_col)
            self.change_turn()