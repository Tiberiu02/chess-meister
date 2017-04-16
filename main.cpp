# include <iostream>
# include <fstream>

# include <vector>
# include <cstring>

# include <algorithm>

# include <ctime>

using namespace std;

/// ********** CONFIG **********

# define my_color Table::Black
# define Depth 7

/// ****************************

class Table
{
public:
    Table( void );

    inline int slot( int x, int y ) const;
    inline void pop( int x, int y );
    inline void push( int val, int x, int y );
    inline void toggle( void );
    inline int eval( void ) const;
    vector<Table> next_move( void ) const;
    bool isCheck( void ) const;
    bool isCheckMate( void ) const;

    bool to_move;

    static const int Free = 0;
    static const int Pawn = 1;
    static const int Knight = 2;
    static const int Bishop = 3;
    static const int Rook = 4;
    static const int Queen = 5;
    static const int King = 6;

    static const int White = 0;
    static const int Black = 1;

    static const int won = 100000;
    static const int lost = -100000;
    static const int draw = 0;

    unsigned int t[8];

    int score[2];
    bool king[2];

    static const int Futility = 300;
    static const int DeepFutility = 500;

private:
    inline void set_slot( int val, int x, int y );

    short king_x[2];
    short king_y[2];

    static const int knight_dx[];
    static const int knight_dy[];

    static const int line_dx[];
    static const int line_dy[];

    void move_pawn( int x, int y, vector<Table> &v ) const;
    void move_piece( int x, int y, const int* p_dx, const int* p_dy, int n, bool mul, vector<Table> &v ) const;

    static const int bonus[8][64];
    static const int value[7];
};

Table::Table( void )
{
    to_move = 0;
    memset( t, 0, 32 );
    king[0] = king[1] = 0;
    score[0] = score[1] = 0;
}

inline void Table::toggle( void )
{
    to_move ^= 1;
}

inline int Table::slot( int x, int y ) const
{
    return t[x] >> ( y << 2 ) & 15;
}

inline void Table::set_slot( int val, int x, int y )
{
    t[x] ^= ( slot( x, y ) ^ val ) << ( y << 2 );
}

const int Table::bonus[8][64] =
{
    {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0
    },
    { /// Pawn
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    },
    { /// Knight
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    },
    { /// Bishop
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    },
    { /// Rooks
          0,  0,  0,  0,  0,  0,  0,  0,
          5, 10, 10, 10, 10, 10, 10,  5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          0,  0,  0,  5,  5,  0,  0,  0
    },
    { /// Queen
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    { /// King middle game
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    },
    { /// King end game
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    }
};
const int Table::value[7] = { 0, 100, 320, 330, 500, 900, 20000 };

int rvs[2][8] = { { 0, 1, 2, 3, 4, 5, 6, 7 }, { 7, 6, 5, 4, 3, 2, 1, 0 } };
inline int Table::eval( void ) const
{
    return to_move == 0 ? score[0] - score[1] : score[1] - score[0];
}

inline bool Table::isCheckMate() const
{
    if ( !isCheck() )
        return false;

    vector<Table> aux = next_move();

    int i;
    for ( i = 0; i < aux.size(); i ++ ) {
        Table &m = aux[i];
        m.toggle();

        if ( !m.isCheck() )
            return false;
    }

    if ( i == aux.size() )
        return true;
}

inline void Table::pop( int x, int y )
{
    int t = slot( x, y );

    if ( t == Free )
        return;

    int p = ( t >> 1 );
    int c = ( t & 1 );

    if ( p == King )
        king[c] = 0;

    score[c] -= value[p] + bonus[p][( rvs[c][x] << 3 ) + y];

    set_slot( Free, x, y );
}

inline void Table::push( int val, int x, int y )
{
    if ( slot( x, y ) != Free )
        pop( x, y );

    if ( val == 0 )
        return;

    int p = ( val >> 1 );
    int c = ( val & 1 );

    if ( p == King ) {
        king[c] = 1;
        king_x[c] = x;
        king_y[c] = y;
    }

    score[c] += value[p] + bonus[p][( rvs[c][x] << 3 ) + y];

    set_slot( val, x, y );
}

inline vector<Table> Table::next_move( void ) const
{
    vector<Table> pos;

    for ( int x = 0; x < 8; x ++ )
        for ( int y = 0; y < 8; y ++ ) {
            int p = slot( x, y );

            if ( p % 2 == to_move ) {
                switch ( p / 2 ) {
                case Pawn:
                    move_pawn( x, y, pos );
                    break;

                case Knight:
                    move_piece( x, y, knight_dx, knight_dy, 4, 0, pos );
                    break;

                case Bishop:
                    move_piece( x, y, line_dx, line_dy, 4, 1, pos );
                    break;

                case Rook:
                    move_piece( x, y, line_dx + 4, line_dy + 4, 4, 1, pos );
                    break;

                case Queen:
                    move_piece( x, y, line_dx, line_dy, 8, 1, pos );
                    break;

                case King:
                    move_piece( x, y, line_dx, line_dy, 8, 0, pos );
                    break;
                }
            }
        }

    return pos;
}

const int Table::knight_dx[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
const int Table::knight_dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };

const int Table::line_dx[] = { -1, 1, 1, -1, -1, 0, 1, 0 };
const int Table::line_dy[] = { 1, 1, -1, -1, 0, 1, 0, -1 };
void Table::move_piece( int x, int y, const int* p_dx, const int* p_dy, int n, bool mul, vector<Table> &v ) const
{
    const int p = slot( x, y );

    for ( int i = 0; i < n; i ++ ) {
        const int &dx = p_dx[i];
        const int &dy = p_dy[i];

        int newX = x + dx;
        int newY = y + dy;

        if ( mul ) {
            while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
            && slot( newX, newY ) == Free ) {
                Table t = *this;

                t.pop( x, y );
                t.push( p, newX, newY );
                t.toggle();

                v.push_back( t );

                newX += dx;
                newY += dy;
            }

            if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
            && ( slot( newX, newY ) & 1 ) != ( p & 1 ) ) {
                Table t = *this;

                t.pop( x, y );
                t.push( p, newX, newY );
                t.toggle();

                v.push_back( t );
            }
        } else {
            if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
            && ( slot( newX, newY ) == Free || ( slot( newX, newY ) & 1 ) != ( p & 1 ) ) ) {
                Table t = *this;

                t.pop( x, y );
                t.push( p, newX, newY );
                t.toggle();

                v.push_back( t );
            }
        }
    }
}

void Table::move_pawn( int x, int y, vector<Table> &v ) const
{
    int p = slot( x, y );

    int newX = x + ( ( p % 2 ) ? 1 : -1 );
    int newP = ( ( newX == 0 || newX == 7 ) ? ( Queen * 2 + p % 2 ) : p );

    if ( slot( newX, y ) == Free ) {
        Table t = *this;

        t.pop( x, y );
        t.push( newP, newX, y );
        t.toggle();

        v.push_back( t );
    }

    for ( int newY = y - 1; newY <= y + 1; newY += 2 )
        if ( newY >= 0 && newY < 8
        && slot( newX, newY ) != Free
        && slot( newX, newY ) % 2 != newP % 2 ) {
            Table t = *this;

            t.pop( x, y );
            t.pop( newX, newY );
            t.push( newP, newX, newY );
            t.toggle();

            v.push_back( t );
        }
}

bool Table::isCheck( void ) const
{
    int x = king_x[to_move];
    int y = king_y[to_move];

    for ( int i = 0; i < 8; i ++ ) {
        int newX = x + knight_dx[i];
        int newY = y + knight_dy[i];

        if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        && ( ( slot( newX, newY ) >> 1 ) == Knight && ( slot( newX, newY ) & 1 ) != to_move ) )
            return true;
    }

    for ( int i = 0; i < 4; i ++ ) {
        const int &dx = line_dx[4 + i];
        const int &dy = line_dy[4 + i];

        int newX = x + dx;
        int newY = y + dy;

        while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        && slot( newX, newY ) == Free ) {
            newX += dx;
            newY += dy;
        }

        if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        && ( slot( newX, newY ) & 1 ) != to_move
        && ( ( slot( newX, newY ) >> 1 ) == Rook || ( slot( newX, newY ) >> 1 ) == Queen ) ) {
            return true;
        }
    }

    for ( int i = 0; i < 4; i ++ ) {
        const int &dx = line_dx[i];
        const int &dy = line_dy[i];

        int newX = x + dx;
        int newY = y + dy;

        while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        && slot( newX, newY ) == Free ) {
            newX += dx;
            newY += dy;
        }

        if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        && ( slot( newX, newY ) & 1 ) != to_move
        && ( ( slot( newX, newY ) >> 1 ) == Bishop || ( slot( newX, newY ) >> 1 ) == Queen ) ) {
            return true;
        }
    }

    return false;
}

struct Move
{
    int score;
    Table t;

    Move( int a, Table b ) { score = a; t = b; }
    bool operator<( const Move b ) { return this->score > b.score; }
    bool operator<( Move b ) const { return this->score > b.score; }
};

int negamax( int d, Table &t, int alpha, int beta )
{
    if ( t.isCheckMate() )
        return Table::lost - d;

    if ( d == 0 )
        return t.eval();
    else if ( d == 1 ) {
        /// Futility Pruning
        if ( !t.isCheck() && t.eval() + Table::Futility <= alpha )
            return alpha;

        bool any_valid_move = false;
        vector<Table> aux = t.next_move();

        for ( int i = 0; i < aux.size(); i ++ ) {
            Table &m = aux[i];

            m.toggle();
            if ( m.isCheck() )
                continue;
            m.toggle();

            any_valid_move = true;
            alpha = max( alpha, -negamax( 0, m, alpha, beta ) );
            if ( beta <= alpha )
                return alpha;
        }

        t.toggle();
        if ( !any_valid_move )
            return Table::draw;

        return alpha;
    } else {
        /// Deep Futility Pruning
        if ( d == 2 && !t.isCheck() && t.eval() + Table::DeepFutility <= alpha )
            return alpha;

        vector<Move> next_move;
        vector<Table> aux = t.next_move();

        for ( int i = 0; i < aux.size(); i ++ ) {
            Table &m = aux[i];

            m.toggle();
            if ( m.isCheck() )
                continue;
            m.toggle();

            next_move.push_back( Move( -m.eval(), m ) );
        }

        t.toggle();
        if ( !next_move.size() )
            return Table::draw;
        t.toggle();

        sort( next_move.begin(), next_move.begin() + next_move.size() );

        for ( int i = 0; i < next_move.size(); i ++ ) {
            Move &n = next_move[i];
            Table &m = n.t;

            alpha = max( alpha, -negamax( d - 1, m, -beta, -alpha ) );
            if ( beta <= alpha )
                return alpha;
        }

        return alpha;
    }
}

template <typename T>
void print_table( Table &best, T &out )
{
    char dconv[8] = { '_', 'P', 'N', 'B', 'R', 'Q', 'K' };
    for ( int x = 0; x < 8; x ++ ) {
        for ( int y = 0; y < 8; y ++ ) {
            int p = best.slot( x, y );
            out << dconv[p / 2] << p%2 << ' ';
        }
        out << endl;
    }
    out << endl;
}

int main()
{
    Table t;

    t.to_move = my_color;

    int conv[128];
    conv['_'] = 0;
    conv['P'] = Table::Pawn;
    conv['N'] = Table::Knight;
    conv['B'] = Table::Bishop;
    conv['R'] = Table::Rook;
    conv['Q'] = Table::Queen;
    conv['K'] = Table::King;

    ifstream fin( "table.in" );
    for ( int x = 0; x < 8; x ++ ) {
        for ( int y = 0; y < 8; y ++ ) {
            string str;

            fin >> str;

            t.push( conv[str[0]] * 2 + str[1] - '0', x, y );
        }
    }
    fin.close();

    Move best_move( 0, Table() );

    int d = 0;
    while ( d < Depth ) {
        int alpha = -100000000;
        int beta = 100000000;

        vector<Move> layer;
        vector<Table> aux = t.next_move();

        for ( int i = 0; i < aux.size(); i ++ ) {
            Table &m = aux[i];

            m.toggle();
            if ( m.isCheck() )
                continue;
            m.toggle();

            int f = -negamax( d, m, -beta, -alpha );
            if ( f > alpha )
                alpha = f;

            layer.push_back( Move( f, m ) );
        }

        best_move = layer[0];
        for ( int i = 1; i < layer.size(); i ++ )
            if ( layer[i] < best_move )
                best_move = layer[i];

        cout << "Depth " << ++ d << " done in " << clock() / 1000.0 << " : " << best_move.score << endl;
    }

    ofstream fout( "table.out" );
    print_table( best_move.t, fout );
    fout.close();

    return 0;
}
