import random
from constants import *

class ChessAI:
    def __init__(self, depth=3):
        self.depth = depth
        self.transposition_table = {}

    def evaluate_board(self, board):
        """Evaluates the board from the perspective of the white player."""
        score = 0
        for row in range(ROWS):
            for col in range(COLS):
                piece = board.get_piece(row, col)
                if piece:
                    piece_value = PIECE_VALUES[piece.type]
                    pos_table = POSITION_TABLES[piece.type]
                    if piece.color == 'w':
                        pos_bonus = pos_table[row][col]
                        score += piece_value + pos_bonus
                    else:
                        pos_bonus = pos_table[7-row][col]
                        score -= (piece_value + pos_bonus)
        return score

    def get_all_moves(self, board, color):
        all_moves = []
        for piece in board.get_all_pieces(color):
            valid_moves = board.get_valid_moves(piece)
            for move in valid_moves:
                all_moves.append((piece, move))
        return all_moves

    def minimax(self, board, depth, alpha, beta, maximizing_player):
        if depth == 0 or board.is_checkmate('w') or board.is_checkmate('b') or board.is_stalemate('w') or board.is_stalemate('b'):
            return self.evaluate_board(board), None
        
        best_move = None
        
        if maximizing_player: # White's turn
            max_eval = float('-inf')
            moves = self.get_all_moves(board, 'w')
            random.shuffle(moves) # Add randomness for varied play

            for piece, move in moves:
                old_row, old_col = piece.row, piece.col
                original_moved_status = piece.moved
                captured_piece = board.move_piece(piece, move[0], move[1])

                eval_score, _ = self.minimax(board, depth - 1, alpha, beta, False)
                
                # Undo move
                board.undo_move() # Simplified undo using history
                piece.moved = original_moved_status # Restore moved status

                if eval_score > max_eval:
                    max_eval = eval_score
                    best_move = (piece, move)
                
                alpha = max(alpha, eval_score)
                if beta <= alpha:
                    break
            return max_eval, best_move
        else: # Black's turn
            min_eval = float('inf')
            moves = self.get_all_moves(board, 'b')
            random.shuffle(moves)

            for piece, move in moves:
                old_row, old_col = piece.row, piece.col
                original_moved_status = piece.moved
                captured_piece = board.move_piece(piece, move[0], move[1])
                
                eval_score, _ = self.minimax(board, depth - 1, alpha, beta, True)

                # Undo move
                board.undo_move()
                piece.moved = original_moved_status
                
                if eval_score < min_eval:
                    min_eval = eval_score
                    best_move = (piece, move)
                
                beta = min(beta, eval_score)
                if beta <= alpha:
                    break
            return min_eval, best_move

    def get_best_move(self, board, color):
        _, best_move = self.minimax(board, self.depth, float('-inf'), float('inf'), False)
        return best_move