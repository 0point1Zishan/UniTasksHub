import pygame
from constants import *
from piece import Piece, Pawn, Rook, Knight, Bishop, Queen, King

class Board:
    def __init__(self):
        self.board = [[None for _ in range(COLS)] for _ in range(ROWS)]
        self.create_board()
        self.move_history = []
        # Store captured pieces and moved statuses for robust undo
        self.history_details = []

    def create_board(self):
        # Setup initial board state
        self.board[0] = [Rook(0, 0, 'b'), Knight(0, 1, 'b'), Bishop(0, 2, 'b'), Queen(0, 3, 'b'), King(0, 4, 'b'), Bishop(0, 5, 'b'), Knight(0, 6, 'b'), Rook(0, 7, 'b')]
        self.board[1] = [Pawn(1, i, 'b') for i in range(COLS)]
        self.board[6] = [Pawn(6, i, 'w') for i in range(COLS)]
        self.board[7] = [Rook(7, 0, 'w'), Knight(7, 1, 'w'), Bishop(7, 2, 'w'), Queen(7, 3, 'w'), King(7, 4, 'w'), Bishop(7, 5, 'w'), Knight(7, 6, 'w'), Rook(7, 7, 'w')]

    def draw(self, win):
        self.draw_squares(win)
        for row in range(ROWS):
            for col in range(COLS):
                piece = self.board[row][col]
                if piece:
                    piece.draw(win)

    def draw_squares(self, win):
        for row in range(ROWS):
            for col in range(COLS):
                color = LIGHT_SQ if (row + col) % 2 == 0 else DARK_SQ
                pygame.draw.rect(win, color, (col * SQ_SIZE, row * SQ_SIZE, SQ_SIZE, SQ_SIZE))

    def get_piece(self, row, col):
        if 0 <= row < ROWS and 0 <= col < COLS:
            return self.board[row][col]
        return None

    def move_piece(self, piece, new_row, new_col):
        old_row, old_col = piece.row, piece.col
        captured_piece = self.get_piece(new_row, new_col)
        
        # Store state for undo
        history_item = {
            "piece": piece,
            "old_pos": (old_row, old_col),
            "new_pos": (new_row, new_col),
            "captured": captured_piece,
            "piece_moved_status": piece.moved,
            "was_promotion": False,
            "castle_details": None
        }

        # Handle castling
        if piece.type == 'K' and abs(new_col - old_col) == 2:
            rook_col_start = 7 if new_col > old_col else 0
            rook_col_end = 5 if new_col > old_col else 3
            rook = self.get_piece(old_row, rook_col_start)
            if rook:
                history_item["castle_details"] = {
                    "rook": rook, 
                    "rook_moved_status": rook.moved
                }
                self.board[old_row][rook_col_start] = None
                self.board[old_row][rook_col_end] = rook
                rook.row, rook.col = old_row, rook_col_end
                rook.moved = True

        self.board[old_row][old_col] = None
        self.board[new_row][new_col] = piece
        piece.row, piece.col = new_row, new_col
        piece.moved = True

        # Handle pawn promotion
        if piece.type == 'P' and (new_row == 0 or new_row == 7):
            promoted_queen = Queen(new_row, new_col, piece.color)
            promoted_queen.moved = True
            self.board[new_row][new_col] = promoted_queen
            history_item["was_promotion"] = True
        
        self.history_details.append(history_item)
        return captured_piece

    def undo_move(self):
        if not self.history_details:
            return
            
        last_move = self.history_details.pop()
        piece = last_move["piece"]
        old_row, old_col = last_move["old_pos"]
        new_row, new_col = last_move["new_pos"]
        captured_piece = last_move["captured"]
        
        # Move piece back
        self.board[old_row][old_col] = piece
        piece.row, piece.col = old_row, old_col
        piece.moved = last_move["piece_moved_status"]
        
        # If it was a promotion, the piece on board is a Queen, but 'piece' is the original Pawn.
        # The board at new_row, new_col needs the captured piece back.
        self.board[new_row][new_col] = captured_piece

        # Handle undoing castling
        if last_move["castle_details"]:
            rook = last_move["castle_details"]["rook"]
            rook.moved = last_move["castle_details"]["rook_moved_status"]
            
            rook_col_start = 7 if new_col > old_col else 0
            rook_col_end = 5 if new_col > old_col else 3
            
            self.board[old_row][rook_col_start] = rook
            self.board[old_row][rook_col_end] = None
            rook.row, rook.col = old_row, rook_col_start

    def get_all_pieces(self, color):
        pieces = []
        for row in self.board:
            for piece in row:
                if piece and piece.color == color:
                    pieces.append(piece)
        return pieces

    def get_valid_moves(self, piece):
        moves = []
        possible_moves = piece.get_possible_moves(self)
        for new_row, new_col in possible_moves:
            self.move_piece(piece, new_row, new_col)
            if not self.is_in_check(piece.color):
                moves.append((new_row, new_col))
            self.undo_move()
        return moves

    def find_king(self, color):
        for r in range(ROWS):
            for c in range(COLS):
                piece = self.get_piece(r, c)
                if piece and piece.type == 'K' and piece.color == color:
                    return piece
        return None

    def is_square_attacked(self, row, col, attacking_color):
        """
        BUG FIX: This function has been completely rewritten to check for raw attack
        patterns without calling get_possible_moves, which resolves the RecursionError.
        """
        for r in range(ROWS):
            for c in range(COLS):
                p = self.get_piece(r, c)
                if not p or p.color != attacking_color:
                    continue

                # Pawn attacks
                if p.type == 'P':
                    direction = -1 if p.color == 'w' else 1
                    if r + direction == row and abs(c - col) == 1:
                        return True
                # Knight attacks
                elif p.type == 'N':
                    knight_moves = [(r-2, c-1), (r-2, c+1), (r+2, c-1), (r+2, c+1),
                                    (r-1, c-2), (r-1, c+2), (r+1, c-2), (r+1, c+2)]
                    if (row, col) in knight_moves:
                        return True
                # King attacks
                elif p.type == 'K':
                    if max(abs(r - row), abs(c - col)) == 1:
                        return True
                # Sliding piece attacks (Rook, Bishop, Queen)
                else:
                    directions = []
                    if p.type in ('R', 'Q'):
                        directions.extend([(0, 1), (0, -1), (1, 0), (-1, 0)])
                    if p.type in ('B', 'Q'):
                        directions.extend([(1, 1), (1, -1), (-1, 1), (-1, -1)])
                    
                    for dr, dc in directions:
                        for i in range(1, 8):
                            new_row, new_col = r + dr * i, c + dc * i
                            if not (0 <= new_row < ROWS and 0 <= new_col < COLS):
                                break
                            if new_row == row and new_col == col:
                                return True
                            if self.get_piece(new_row, new_col) is not None:
                                break
        return False

    def is_in_check(self, color):
        king = self.find_king(color)
        if king:
            enemy_color = 'b' if color == 'w' else 'w'
            return self.is_square_attacked(king.row, king.col, enemy_color)
        return False

    def is_checkmate(self, color):
        if not self.is_in_check(color):
            return False
        for piece in self.get_all_pieces(color):
            if self.get_valid_moves(piece):
                return False
        return True

    def is_stalemate(self, color):
        if self.is_in_check(color):
            return False
        for piece in self.get_all_pieces(color):
            if self.get_valid_moves(piece):
                return False
        return True