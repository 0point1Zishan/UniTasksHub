import pygame
import os
from constants import *

class Piece:
    def __init__(self, row, col, color, piece_type):
        self.row = row
        self.col = col
        self.color = color
        self.type = piece_type
        self.moved = False
        self.image = None
        self.load_image()

    def load_image(self):
        try:
            # Assumes an 'assets' folder with images like 'wP.png', 'bK.png' etc.
            image_path = os.path.join('assets', f'{self.color}{self.type}.png')
            if os.path.exists(image_path):
                self.image = pygame.image.load(image_path)
                self.image = pygame.transform.scale(self.image, (SQ_SIZE, SQ_SIZE))
            else:
                # This fallback will be used if images are not found.
                pass
        except Exception as e:
            print(f"Error loading image {self.color}{self.type}.png: {e}")
            pass

    def draw(self, win):
        if self.image:
            win.blit(self.image, (self.col * SQ_SIZE, self.row * SQ_SIZE))
        else:
            # Draw text if image fails to load
            font = pygame.font.Font(None, 30)
            text_color = BLACK if self.color == 'w' else WHITE
            text = font.render(f'{self.type}', True, text_color)
            center_x = self.col * SQ_SIZE + SQ_SIZE // 2 - text.get_width() // 2
            center_y = self.row * SQ_SIZE + SQ_SIZE // 2 - text.get_height() // 2
            win.blit(text, (center_x, center_y))

    def copy(self):
        piece_classes = {
            'P': Pawn, 'R': Rook, 'N': Knight,
            'B': Bishop, 'Q': Queen, 'K': King
        }
        new_piece = piece_classes[self.type](self.row, self.col, self.color)
        new_piece.moved = self.moved
        return new_piece

    def get_possible_moves(self, board):
        """Generates pseudo-legal moves. This will be overridden in subclasses."""
        return []

class Pawn(Piece):
    def __init__(self, row, col, color):
        super().__init__(row, col, color, 'P')

    def get_possible_moves(self, board):
        moves = []
        direction = -1 if self.color == 'w' else 1
        start_row = 6 if self.color == 'w' else 1

        # Forward move
        new_row = self.row + direction
        if 0 <= new_row < ROWS and board.get_piece(new_row, self.col) is None:
            moves.append((new_row, self.col))
            # Double move from starting position
            if self.row == start_row and board.get_piece(new_row + direction, self.col) is None:
                moves.append((new_row + direction, self.col))

        # Diagonal captures
        for col_offset in [-1, 1]:
            new_col = self.col + col_offset
            if 0 <= new_col < COLS and 0 <= new_row < ROWS:
                target = board.get_piece(new_row, new_col)
                if target and target.color != self.color:
                    moves.append((new_row, new_col))
        
        return moves

class Rook(Piece):
    def __init__(self, row, col, color):
        super().__init__(row, col, color, 'R')

    def get_possible_moves(self, board):
        moves = []
        directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]
        for dr, dc in directions:
            for i in range(1, 8):
                new_row, new_col = self.row + dr * i, self.col + dc * i
                if not (0 <= new_row < ROWS and 0 <= new_col < COLS):
                    break
                target = board.get_piece(new_row, new_col)
                if target is None:
                    moves.append((new_row, new_col))
                elif target.color != self.color:
                    moves.append((new_row, new_col))
                    break
                else:
                    break
        return moves

class Knight(Piece):
    def __init__(self, row, col, color):
        super().__init__(row, col, color, 'N')

    def get_possible_moves(self, board):
        """BUG FIX: Implemented correct L-shaped knight moves."""
        moves = []
        possible_moves = [
            (self.row - 2, self.col + 1), (self.row - 2, self.col - 1),
            (self.row + 2, self.col + 1), (self.row + 2, self.col - 1),
            (self.row - 1, self.col + 2), (self.row - 1, self.col - 2),
            (self.row + 1, self.col + 2), (self.row + 1, self.col - 2)
        ]
        for r, c in possible_moves:
            if 0 <= r < ROWS and 0 <= c < COLS:
                target = board.get_piece(r, c)
                if target is None or target.color != self.color:
                    moves.append((r, c))
        return moves

class Bishop(Piece):
    def __init__(self, row, col, color):
        super().__init__(row, col, color, 'B')

    def get_possible_moves(self, board):
        moves = []
        directions = [(1, 1), (1, -1), (-1, 1), (-1, -1)]
        for dr, dc in directions:
            for i in range(1, 8):
                new_row, new_col = self.row + dr * i, self.col + dc * i
                if not (0 <= new_row < ROWS and 0 <= new_col < COLS):
                    break
                target = board.get_piece(new_row, new_col)
                if target is None:
                    moves.append((new_row, new_col))
                elif target.color != self.color:
                    moves.append((new_row, new_col))
                    break
                else:
                    break
        return moves

class Queen(Piece):
    def __init__(self, row, col, color):
        super().__init__(row, col, color, 'Q')

    def get_possible_moves(self, board):
        # Combines Rook and Bishop moves
        rook = Rook(self.row, self.col, self.color)
        bishop = Bishop(self.row, self.col, self.color)
        return rook.get_possible_moves(board) + bishop.get_possible_moves(board)

class King(Piece):
    def __init__(self, row, col, color):
        super().__init__(row, col, color, 'K')

    def get_possible_moves(self, board):
        """BUG FIX: Correct king move logic, including castling, moved here."""
        moves = []
        directions = [(0, 1), (0, -1), (1, 0), (-1, 0),
                      (1, 1), (1, -1), (-1, 1), (-1, -1)]

        # Normal one-square moves
        for dr, dc in directions:
            new_row, new_col = self.row + dr, self.col + dc
            if 0 <= new_row < ROWS and 0 <= new_col < COLS:
                target = board.get_piece(new_row, new_col)
                if target is None or target.color != self.color:
                    moves.append((new_row, new_col))

        # Castling moves
        if not self.moved and not board.is_in_check(self.color):
            enemy_color = 'b' if self.color == 'w' else 'w'
            # Kingside castling
            kingside_rook = board.get_piece(self.row, 7)
            if (kingside_rook and kingside_rook.type == 'R' and not kingside_rook.moved):
                if board.get_piece(self.row, 5) is None and board.get_piece(self.row, 6) is None:
                    if (not board.is_square_attacked(self.row, 5, enemy_color) and
                        not board.is_square_attacked(self.row, 6, enemy_color)):
                        moves.append((self.row, 6))

            # Queenside castling
            queenside_rook = board.get_piece(self.row, 0)
            if (queenside_rook and queenside_rook.type == 'R' and not queenside_rook.moved):
                if (board.get_piece(self.row, 1) is None and
                    board.get_piece(self.row, 2) is None and
                    board.get_piece(self.row, 3) is None):
                    if (not board.is_square_attacked(self.row, 2, enemy_color) and
                        not board.is_square_attacked(self.row, 3, enemy_color)):
                        moves.append((self.row, 2))
        return moves