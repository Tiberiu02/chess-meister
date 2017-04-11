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

    unsigned long long t[4];

    inline int king( int p ) const;

    static const int Futility = 300;
    static const int DeepFutility = 500;

private:
    inline void set_slot( int val, int x, int y );

    long long stats;
    short king_x[2];
    short king_y[2];

    inline int score( int p ) const;
    inline void set_score( int val, int p );
    inline void set_king( int val, int p );

    static const int knight_dx[];
    static const int knight_dy[];

    static const int bishop_dx[];
    static const int bishop_dy[];

    static const int rook_dx[];
    static const int rook_dy[];

    static const int queen_dx[];
    static const int queen_dy[];

    static const int king_dx[];
    static const int king_dy[];

    void move_pawn( int x, int y, vector<Table> &v ) const;
    void move_knight( int x, int y, vector<Table> &v ) const;
    void move_bishop( int x, int y, vector<Table> &v ) const;
    void move_rook( int x, int y, vector<Table> &v ) const;
    void move_queen( int x, int y, vector<Table> &v ) const;
    void move_king( int x, int y, vector<Table> &v ) const;

    static const int bonus[8][64];
    static const int value[7];
};

Table::Table( void )
{
    to_move = 0;
    t[0] = t[1] = t[2] = t[3] = 0;
    stats = 0;
}

inline int Table::score( int p ) const
{
    return ( stats >> ( 2 | ( p << 4 ) ) ) & ( ( 1 << 16 ) - 1 );
}

inline void Table::set_score( int val, int p )
{
    stats = stats ^ ( (long long)score( p ) << ( 2 | ( p << 4 ) ) ) | ( (long long)val << ( 2 | ( p << 4 ) ) );
}

inline int Table::king( int p ) const
{
    return ( stats >> p ) & 1;
}

inline void Table::set_king( int val, int p )
{
    stats = stats ^ ( stats & 1 << p ) | ( val << p );
}

inline void Table::toggle( void )
{
    to_move ^= 1;
}

inline int Table::slot( int x, int y ) const
{
    return ( t[x >> 1] >> ( ( y << 2 ) | ( ( x & 1 ) << 5 ) ) ) & 15;
}

inline void Table::set_slot( int val, int x, int y )
{
    t[x >> 1] = ( t[x >> 1] ^ ( (long long)slot( x, y ) << ( ( y << 2 ) | ( ( x & 1 ) << 5 ) ) ) )
                | ( (long long)val << ( ( y << 2 ) | ( ( x & 1 ) << 5 ) ) );
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
int sign[2] = { 1, -1 };
inline int Table::eval( void ) const
{
    return sign[to_move] * ( score( 0 ) - score( 1 ) );
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
        set_king( 0, c );

    set_score( score( c ) - value[p] - bonus[p][( rvs[c][x] << 3 ) + y], c );

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
        set_king( 1, c );
        king_x[c] = x;
        king_y[c] = y;
    }

    set_score( score( c ) + value[p] + bonus[p][( rvs[c][x] << 3 ) + y], c );

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
                    move_knight( x, y, pos );
                    break;

                case Bishop:
                    move_bishop( x, y, pos );
                    break;

                case Rook:
                    move_rook( x, y, pos );
                    break;

                case Queen:
                    move_queen( x, y, pos );
                    break;

                case King:
                    move_king( x, y, pos );
                    break;
                }
            }
        }

    return pos;
}

const int Table::king_dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int Table::king_dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
void Table::move_king( int x, int y, vector<Table> &v ) const
{
    int p = slot( x, y );

    for ( int i = 0; i < 8; i ++ ) {
        int newX = x + king_dx[i];
        int newY = y + king_dy[i];

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

const int Table::queen_dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int Table::queen_dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
void Table::move_queen( int x, int y, vector<Table> &v ) const
{
    int p = slot( x, y );

    for ( int i = 0; i < 8; i ++ ) {
        const int &dx = queen_dx[i];
        const int &dy = queen_dy[i];

        int newX = x + dx;
        int newY = y + dy;

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
    }
}

const int Table::rook_dx[] = { -1, 0, 1, 0 };
const int Table::rook_dy[] = { 0, 1, 0, -1 };
void Table::move_rook( int x, int y, vector<Table> &v ) const
{
    int p = slot( x, y );

    for ( int i = 0; i < 4; i ++ ) {
        const int &dx = rook_dx[i];
        const int &dy = rook_dy[i];

        int newX = x + dx;
        int newY = y + dy;

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
    }
}

const int Table::bishop_dx[] = { -1, 1, 1, -1 };
const int Table::bishop_dy[] = { 1, 1, -1, -1 };
void Table::move_bishop( int x, int y, vector<Table> &v ) const
{
    int p = slot( x, y );

    for ( int i = 0; i < 4; i ++ ) {
        const int &dx = bishop_dx[i];
        const int &dy = bishop_dy[i];

        int newX = x + dx;
        int newY = y + dy;

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
    }
}

const int Table::knight_dx[] = { -2, -1,  1,  2,  2,  1, -1, -2 };
const int Table::knight_dy[] = {  1,  2,  2,  1, -1, -2, -2, -1 };
void Table::move_knight( int x, int y, vector<Table> &v ) const
{
    int p = slot( x, y );

    for ( int i = 0; i < 8; i ++ ) {
        int newX = x + knight_dx[i];
        int newY = y + knight_dy[i];

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
        const int &dx = rook_dx[i];
        const int &dy = rook_dy[i];

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
        const int &dx = bishop_dx[i];
        const int &dy = bishop_dy[i];

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

    vector<Move> layer;
    vector<Table> aux;

    for ( int i = 0; i < aux.size(); i ++ ) {
        Table &m = aux[i];

        if ( m.isCheck() )
            continue;

        layer.push_back( Move( m.eval(), m ) );
    }

    sort( layer.begin(), layer.begin() + layer.size() );

    cout << "Depth 1 done" << endl;

    int d = 0;
    while ( d < Depth ) {
        int alpha = -100000000;
        int beta = 100000000;

        vector<Move> new_layer;
        vector<Table> aux = t.next_move();

        for ( int i = 0; i < aux.size(); i ++ ) {
            Table &m = aux[i];

            m.toggle();
            if ( m.isCheck() )
                continue;
            m.toggle();

            int f = -negamax( d, m, -beta, -alpha );
            if ( f > alpha && d < beta )
                alpha = f;

            new_layer.push_back( Move( f, m ) );
        }

        layer = new_layer;
        stable_sort( layer.begin(), layer.begin() + layer.size() );


        cout << "Depth " << ++ d << " done in " << clock() / 1000.0 << " : " << layer[0].score << endl;
    }

    ofstream fout( "table.out" );
    print_table( layer[0].t, fout );
    fout.close();

    return 0;
}
