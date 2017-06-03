# include <iostream>
# include <fstream>

# include <vector>
# include <cstring>

# include <algorithm>

# include <ctime>

using namespace std;

/// ********** CONFIG **********

# define my_color Black
# define Depth 9

/// ****************************

struct Table {
	bool to_move;
	unsigned int t[8];
	int score;
	int king_x[2], king_y[2];
};

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

static const int Futility = 900;
static const int DeepFutility = 1500;

static inline Table new_table();
static inline void toggle( Table& a );
static inline int slot( const Table& a, int x, int y );
static inline void set_slot( Table& a, int val, int x, int y );
static inline void pop( Table& a, int x, int y );
static inline void push( Table& a, int val, int x, int y );
static inline int eval( const Table& a );
static inline bool isCheckMate( const Table& a );
static inline bool isCheck( const Table& a );
static inline vector<Table> next_move( const Table& a );
static inline void move_piece( const Table& a, int x, int y, const int* p_dx, const int* p_dy, int n, bool mul, vector<Table> &v );
static inline void move_pawn( const Table& a, int x, int y, vector<Table> &v );

static const int bonus[8][64] =
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
      500,500,500,500,500,500,500,500,
      110,110,120,130,130,120,110,110,
       25, 25, 30, 45, 45, 30, 25, 25,
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
static const int value[7] = { 0, 100, 320, 330, 500, 900, 20000 };

static inline Table new_table()
{
	Table a;

	a.to_move = 0;
	memset( a.t, 0, 32 );
	a.score = 0;

	return a;
}

static inline void toggle( Table& a )
{
	a.to_move ^= 1;
	a.score *= -1;
}

static inline int slot( const Table& a, int x, int y )
{
	return a.t[x] >> ( y << 2 ) & 15;
}

static inline void set_slot( Table& a, int val, int x, int y )
{
	a.t[x] ^= ( slot( a, x, y ) ^ val ) << ( y << 2 );
}

# define rvs( a, b ) ( a == 0 ? b : 7 - b )
static inline int eval( const Table& a )
{
	return a.score;
}

static inline bool isCheckMate( const Table& a )
{
	if ( !isCheck( a ) )
    	return false;

	vector<Table> aux = next_move( a );

	int i;
	for ( i = 0; i < aux.size(); i ++ ) {
    	Table &m = aux[i];
    	toggle( m );

    	if ( !isCheck( m ) )
        	return false;
	}

    return true;
}

static inline void pop( Table& a, int x, int y )
{
	int t = slot( a, x, y );

	if ( t == Free )
    	return;

	int p = ( t >> 1 );
	int c = ( t & 1 );

    if ( c == a.to_move )
        a.score -= value[p] + bonus[p][( rvs( c, x ) << 3 ) + y];
    else
        a.score += value[p] + bonus[p][( rvs( c, x ) << 3 ) + y];

	set_slot( a, Free, x, y );
}

static inline void push( Table& a, int val, int x, int y )
{
	if ( slot( a, x, y ) != Free )
    	pop( a, x, y );

	if ( val == 0 )
    	return;

	int p = ( val >> 1 );
	int c = ( val & 1 );

	if ( p == King ) {
    	a.king_x[c] = x;
    	a.king_y[c] = y;
	}

    if ( c == a.to_move )
        a.score += value[p] + bonus[p][( rvs( c, x ) << 3 ) + y];
    else
        a.score -= value[p] + bonus[p][( rvs( c, x ) << 3 ) + y];

	set_slot( a, val, x, y );
}


static const int knight_dx[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
static const int knight_dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };

static const int line_dx[] = { -1, 1, 1, -1, -1, 0, 1, 0 };
static const int line_dy[] = { 1, 1, -1, -1, 0, 1, 0, -1 };
static inline vector<Table> next_move( const Table& a )
{
	vector<Table> pos;

	for ( int x = 0; x < 8; x ++ )
    	for ( int y = 0; y < 8; y ++ ) {
        	int p = slot( a, x, y );

        	if ( p % 2 == a.to_move ) {
            	switch ( p / 2 ) {
            	case Pawn:
                	move_pawn( a, x, y, pos );
                	break;

            	case Knight:
                	move_piece( a, x, y, knight_dx, knight_dy, 4, 0, pos );
                	break;

            	case Bishop:
                	move_piece( a, x, y, line_dx, line_dy, 4, 1, pos );
                	break;

            	case Rook:
                	move_piece( a, x, y, line_dx + 4, line_dy + 4, 4, 1, pos );
                	break;

            	case Queen:
                	move_piece( a, x, y, line_dx, line_dy, 8, 1, pos );
                	break;

            	case King:
                	move_piece( a, x, y, line_dx, line_dy, 8, 0, pos );
                	break;
            	}
        	}
    	}

	return pos;
}
static inline void move_piece( const Table& a, int x, int y, const int* p_dx, const int* p_dy, int n, bool mul, vector<Table> &v )
{
	const int p = slot( a, x, y );

	for ( int i = 0; i < n; i ++ ) {
    	const int &dx = p_dx[i];
    	const int &dy = p_dy[i];

    	int newX = x + dx;
    	int newY = y + dy;

    	if ( mul ) {
        	while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        	&& slot( a, newX, newY ) == Free ) {
            	Table t = a;

            	pop( t, x, y );
            	push( t, p, newX, newY );
            	toggle( t );

            	v.push_back( t );

            	newX += dx;
            	newY += dy;
        	}

        	if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        	&& ( slot( a, newX, newY ) & 1 ) != ( p & 1 ) ) {
            	Table t = a;

            	pop( t, x, y );
            	push( t, p, newX, newY );
            	toggle( t );

            	v.push_back( t );
        	}
    	} else {
        	if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
        	&& ( slot( a, newX, newY ) == Free || ( slot( a, newX, newY ) & 1 ) != ( p & 1 ) ) ) {
            	Table t = a;

            	pop( t, x, y );
            	push( t, p, newX, newY );
            	toggle( t );

            	v.push_back( t );
        	}
    	}
	}
}

static inline void move_pawn( const Table& a, int x, int y, vector<Table> &v )
{
	int p = slot( a, x, y );

	int newX = x + ( ( p % 2 ) ? 1 : -1 );
	int newP = ( ( newX == 0 || newX == 7 ) ? ( Queen * 2 + p % 2 ) : p );

	if ( slot( a, newX, y ) == Free ) {
    	Table t = a;

    	pop( t, x, y );
    	push( t, newP, newX, y );
    	toggle( t );

    	v.push_back( t );
	}

	for ( int newY = y - 1; newY <= y + 1; newY += 2 )
    	if ( newY >= 0 && newY < 8
    	&& slot( a, newX, newY ) != Free
    	&& slot( a, newX, newY ) % 2 != newP % 2 ) {
        	Table t = a;

        	pop( t, x, y );
        	push( t, newP, newX, newY );
        	toggle( t );

        	v.push_back( t );
    	}
}

static inline bool isCheck( const Table& a )
{
	int x = a.king_x[a.to_move];
	int y = a.king_y[a.to_move];

	for ( int i = 0; i < 8; i ++ ) {
    	int newX = x + knight_dx[i];
    	int newY = y + knight_dy[i];

    	if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
    	&& ( slot( a, newX, newY ) == Knight * 2 + !a.to_move ) )
        	return true;
	}

	for ( int i = 0; i < 4; i ++ ) {
    	const int &dx = line_dx[4 + i];
    	const int &dy = line_dy[4 + i];

    	int newX = x + dx;
    	int newY = y + dy;

    	while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
    	&& slot( a, newX, newY ) == Free ) {
        	newX += dx;
        	newY += dy;
    	}

    	if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
    	&& ( slot( a, newX, newY ) == Rook * 2 + !a.to_move || slot( a, newX, newY ) == Queen * 2 + !a.to_move ) ) {
        	return true;
    	}
	}

	for ( int i = 0; i < 4; i ++ ) {
    	const int &dx = line_dx[i];
    	const int &dy = line_dy[i];

    	int newX = x + dx;
    	int newY = y + dy;

    	while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
    	&& slot( a, newX, newY ) == Free ) {
        	newX += dx;
        	newY += dy;
    	}

    	if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
    	&& ( slot( a, newX, newY ) == Bishop * 2 + !a.to_move || slot( a, newX, newY ) == Queen * 2 + !a.to_move ) ) {
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
	inline bool operator<( const Move& b ) const { return this->score > b.score; }
};

inline bool cmp_table( const Table& a, const Table& b )
{
    return a.score < b.score;
}

static inline int negamax( int d, Table &t, int alpha, int beta )
{
	if ( isCheckMate( t ) )
    	return lost - d;

	if ( d == 0 )
    	return eval( t );
	else if ( d == 1 ) {
    	/// Futility Pruning
    	if ( !isCheck( t ) && eval( t ) + Futility <= alpha )
        	return alpha;

    	bool any_valid_move = false;
    	vector<Table> aux = next_move( t );

    	for ( int i = 0; i < aux.size(); i ++ ) {
        	Table &m = aux[i];

        	toggle( m );
        	if ( isCheck( m ) )
            	continue;
        	toggle( m );

        	any_valid_move = true;
        	alpha = max( alpha, -negamax( 0, m, alpha, beta ) );
        	if ( beta <= alpha )
            	return alpha;
    	}

    	if ( !any_valid_move )
        	return draw;

    	return alpha;
	} else {
    	/// Deep Futility Pruning
    	if ( d == 2 && !isCheck( t ) && eval( t ) + DeepFutility <= alpha )
        	return alpha;

    	vector<Table> next_moves;
    	vector<Table> aux = next_move( t );

    	for ( int i = 0; i < aux.size(); i ++ ) {
        	Table &m = aux[i];

        	toggle( m );
        	if ( isCheck( m ) )
            	continue;
        	toggle( m );

        	next_moves.push_back( m );
    	}

    	if ( !next_moves.size() )
        	return draw;

    	sort( next_moves.begin(), next_moves.end(), cmp_table );

    	for ( int i = 0; i < next_moves.size(); i ++ ) {
        	Table &m = next_moves[i];

        	alpha = max( alpha, -negamax( d - 1, m, -beta, -alpha ) );
        	if ( beta <= alpha )
            	return alpha;
    	}

    	return alpha;
	}
}

template <typename T>
void print_table( Table& best, T& out )
{
	char dconv[8] = { '_', 'P', 'N', 'B', 'R', 'Q', 'K' };
	for ( int x = 0; x < 8; x ++ ) {
    	for ( int y = 0; y < 8; y ++ ) {
        	int p = slot( best, x, y );
        	out << dconv[p / 2] << p%2 << ' ';
    	}
    	out << endl;
	}
	out << endl;
}

int main()
{
	Table t = new_table();

	t.to_move = my_color;

	int conv[128];
	conv['_'] = 0;
	conv['P'] = Pawn;
	conv['N'] = Knight;
	conv['B'] = Bishop;
	conv['R'] = Rook;
	conv['Q'] = Queen;
	conv['K'] = King;

	ifstream fin( "table.in" );
	for ( int x = 0; x < 8; x ++ ) {
    	for ( int y = 0; y < 8; y ++ ) {
        	string str;

        	fin >> str;

        	push( t, conv[str[0]] * 2 + str[1] - '0', x, y );
    	}
	}
	fin.close();

	Move best_move( 0, new_table() );

    vector<Move> layer;
    vector<Table> aux;

	int d = 0;
	while ( d < Depth ) {
    	int alpha = -100000000;
    	int beta = 100000000;

        if ( d == 0 )
            aux = next_move( t );
        else {
            aux.clear();
            for ( int i = 0; i < layer.size(); i ++ )
                aux.push_back( layer[i].t );
        }
    	layer.clear();

    	for ( int i = 0; i < aux.size(); i ++ ) {
        	Table &m = aux[i];

        	toggle( m );
        	if ( isCheck( m ) )
            	continue;
        	toggle( m );

        	int f = -negamax( d, m, -beta, -alpha );
        	if ( f > alpha )
            	alpha = f;

        	layer.push_back( Move( f, m ) );
    	}

        sort( layer.begin(), layer.end() );
    	best_move = layer[0];

    	cout << "Depth " << ++ d << " done in " << clock() / 1000.0 << " : " << best_move.score << endl;
	}

	ofstream fout( "table.out" );
	print_table( best_move.t, fout );
	fout.close();

	return 0;
}
