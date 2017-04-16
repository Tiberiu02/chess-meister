<html>
<head>
<meta charset="UTF-8"> 
<title>Chess Game</title>
<style type="text/css">
.chessboard {
    width: 640px;
    height: 640px;
    margin: 20px;
    border: 25px solid #333;
    display: inline-block;
}
.black {
    float: left;
    width: 80px;
    height: 80px;
    background-color: #999;
      font-size:50px;
    text-align:center;
    display: table-cell;
    vertical-align:middle;
}
.white {
    float: left;
    width: 80px;
    height: 80px;
    background-color: #fff;
    font-size:50px;
    text-align:center;
    display: table-cell;
    vertical-align:middle;
}
</style>
<script>

function findGetParameter(parameterName) {
    var result = null,
        tmp = [];
    location.search
    .substr(1)
        .split("&")
        .forEach(function (item) {
        tmp = item.split("=");
        if (tmp[0] === parameterName) result = decodeURIComponent(tmp[1]);
    });
    return result;
}

function tryRestore() {
	if (findGetParameter("restoreTable") != null)
		setTable(findGetParameter("restoreTable"));
}

var knight_dx = [ -2, -1,  1,  2,  2,  1, -1, -2 ];
var knight_dy = [  1,  2,  2,  1, -1, -2, -2, -1 ];
var bishop_dx = [ -1, 1, 1, -1 ];
var bishop_dy = [ 1, 1, -1, -1 ];
var rook_dx = [ -1, 0, 1, 0 ];
var rook_dy = [ 0, 1, 0, -1 ];

var Free = 0;
var Pawn = 1;
var Rook = 2;
var Knight = 3;
var Bishop = 4;
var Queen = 5;
var King = 6;

function valid() {
	for (var x = 0; x < 8; x ++)
		for (var y = 0; y < 8; y ++)
			if (Math.floor(slot(x, y) / 2) == King && slot(x, y) % 2 == 0) {
				var to_move = slot(x, y) % 2;
				
				if ( x != 0 && ( y != 0 && slot(x - 1, y - 1) == Pawn * 2 + 1 || y != 7 && slot(x - 1, y + 1) == Pawn * 2 + 1) )
					return false;

				for ( var i = 0; i < 8; i ++ ) {
				    var newX = x + knight_dx[i];
				    var newY = y + knight_dy[i];

				    if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
				    && ( ( slot( newX, newY ) >> 1 ) == Knight && ( slot( newX, newY ) & 1 ) != to_move ) )
				        return false;
				}

				for ( var i = 0; i < 4; i ++ ) {
				    var dx = rook_dx[i];
				    var dy = rook_dy[i];

				    var newX = x + dx;
				    var newY = y + dy;

				    while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
				    && slot( newX, newY ) == Free ) {
				        newX += dx;
				        newY += dy;
				    }

				    if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
				    && ( slot( newX, newY ) & 1 ) != to_move
				    && ( ( slot( newX, newY ) >> 1 ) == Rook || ( slot( newX, newY ) >> 1 ) == Queen ) ) {
				        return false;
				    }
				}

				for ( var i = 0; i < 4; i ++ ) {
				    var dx = bishop_dx[i];
				    var dy = bishop_dy[i];

				    var newX = x + dx;
				    var newY = y + dy;

				    while ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
				    && slot( newX, newY ) == Free ) {
				        newX += dx;
				        newY += dy;
				    }

				    if ( 0 <= newX && newX < 8 && 0 <= newY && newY < 8
				    && ( slot( newX, newY ) & 1 ) != to_move
				    && ( ( slot( newX, newY ) >> 1 ) == Bishop || ( slot( newX, newY ) >> 1 ) == Queen ) ) {
				        return false;
				    }
				}
			}

		return true;
}

var dragEl;

function get_slot(t) {
	var c = t.className;

	//var x = Math.floor(document.getElementsByClassName(c).indexOf(t) / 8);

	var i = Array.prototype.indexOf.call(document.getElementsByClassName(c), t);
	var x = Math.floor(i / 4);
	var y = i % 4 * 2 + ((c == "white") == x % 2);

	return {x: x, y: y};
}

function allowDrop(ev) {
  ev.preventDefault();
}

function valid_mv(x1, y1, x2, y2) {

	var p = Math.floor(slot(x1, y1) / 2);
	var c = slot(x1, y1) % 2;

	if (p == Pawn) {
		var dx = (c == 1 ? 1 : -1 );

		if (y1 == y2) {
			if (slot(x2, y2) != Free)
				return false;
			if (x1 + 2 * dx == x2)
				return x1 == 1 || x1 == 6;
			if (x1 + dx == x2) {
				if (x2 == 0 || x2 == 7)
					slot_el(x1, y1).innerHTML = '<img src="' + ( c == 0 ? 'w' : 'b' ) + '_queen.png" draggable="true" ondragstart="drag(event)">';	
				return true;
			}
		} else if (Math.abs(y1 - y2) == 1 && x1 + dx == x2 && slot(x2, y2) != Free && slot(x1, y1) % 2 != slot(x2, y2) % 2) {
				if (x2 == 0 || x2 == 7)
					slot_el(x1, y1).innerHTML = '<img src="' + ( c == 0 ? 'w' : 'b' ) + '_queen.png" draggable="true" ondragstart="drag(event)">';	
				return true;
		}
		return false;
	} else if (p == Rook) {
		if ( slot(x2, y2) != Free && slot(x1, y1) % 2 == slot(x2, y2) % 2 )
			return false;

		if (x1 != x2 && y1 != y2)
			return false;

		var dx = ( x1 != x2 ) * ( x1 < x2 ? 1 : -1 );
		var dy = ( y1 != y2 ) * ( y1 < y2 ? 1 : -1 );

		x1 += dx;
		y1 += dy;

		while (x1 != x2 || y1 != y2) {
			if (slot(x1, y1) != Free)
				return false;
		
			x1 += dx;
			y1 += dy;
		} 
	} else if (p == Bishop) {
		if ( slot(x2, y2) != Free && slot(x1, y1) % 2 == slot(x2, y2) % 2 )
			return false;

		if (Math.abs(x1 - x2) != Math.abs(y1 - y2) )
			return false;

		var dx = ( x1 < x2 ? 1 : -1 );
		var dy = ( y1 < y2 ? 1 : -1 );

		x1 += dx;
		y1 += dy;

		while (x1 != x2 || y1 != y2) {
			if (slot(x1, y1) != Free)
				return false;
		
			x1 += dx;
			y1 += dy;
		}
	} else if (p == Queen) {
		if ( slot(x2, y2) != Free && slot(x1, y1) % 2 == slot(x2, y2) % 2 )
			return false;

		if (x1 != x2 && y1 != y2 && Math.abs(x1 - x2) != Math.abs(y1 - y2) )
			return false;

		var dx = ( x1 != x2 ) * ( x1 < x2 ? 1 : -1 );
		var dy = ( y1 != y2 ) * ( y1 < y2 ? 1 : -1 );

		x1 += dx;
		y1 += dy;

		while (x1 != x2 || y1 != y2) {
			if (slot(x1, y1) != Free)
				return false;
		
			x1 += dx;
			y1 += dy;
		}
	} else if (p == King) {
		if ( slot(x2, y2) != Free && slot(x1, y1) % 2 == slot(x2, y2) % 2 )
			return false;

		if (Math.abs(x1 - x2) + Math.abs(y1 - y2) >= 1 && Math.abs(x1 - x2) <= 1 && Math.abs(y1 - y2) <= 1)
			return true;
		else
			return false;
	} else if (p == Knight) {
		if ( slot(x2, y2) != Free && slot(x1, y1) % 2 == slot(x2, y2) % 2 )
			return false;

		var dx = Math.abs(x1 - x2);
		var dy = Math.abs(y1 - y2);
		if ( dx == 2 && dy == 1 || dx == 1 && dy == 2 )
			return true;
		else
			return false;
	}

	return true;
}

var canMove = true;

function setTable(str) {
	var conv = [];
  conv['_'] = 0;
  conv['P'] = "pawn";
  conv['N'] = "knight";
  conv['B'] = "bishop";
  conv['R'] = "rook";
  conv['Q'] = "queen";
  conv['K'] = "king";

	var p = 0;

	for ( var x = 0; x < 8; x ++ )
		for ( var y = 0; y < 8; y ++ ) {
			while ( conv[str[p]] == undefined )
				p ++;
			if (str[p] != '_' )
				slot_el(x, y).innerHTML = '<img src="' + (str[p + 1] == '0' ? 'w' : 'b' ) + '_' + conv[str[p]] + '.png" draggable="true" ondragstart="drag(event)">';
			else
				slot_el(x, y).innerHTML = "";
			p += 2;
		}
}

function getTable() {
	var str = "";
	
	var conv = ['_', 'P', 'R', 'N', 'B', 'Q', 'K' ];
	for ( var x = 0; x < 8; x ++ )
		for ( var y = 0; y < 8; y ++ ) {
			str += conv[slot(x, y) >> 1];
			str += slot(x, y) & 1;
			str += ' ';
		}

	return str;
}

function opponent(oldTable) {
  var table = getTable();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      setTable(this.responseText);
      canMove = true;
      document.getElementById("load").style.display = "none";
    }
  };
  xhttp.open("GET", "do-move.php?table=" + table, true);
  xhttp.send();
  
  setTimeout(function(req, table) {
    if (req.readyState != 4 || req.status != 200) {
      setTable(table);
      req.abort();
      canMove = true;
      document.getElementById("load").style.display = "none";
    }
  }, 5000, xhttp, oldTable);
}

function drop(ev) {
	ev.preventDefault();
	
	var data = dragEl;
	var tg = ev.target;
	var od = data.parentElement;
	var oldTable = getTable();
	
	if (tg.tagName == "IMG")
		tg = tg.parentElement;
	var odc = (' ' + tg.innerHTML).slice(1);
	
	if (tg.tagName != "DIV")
		return;
	
	var x1 = get_slot(od).x;
	var y1 = get_slot(od).y;
	var x2 = get_slot(tg).x;
	var y2 = get_slot(tg).y;
	
	if (slot(x1, y1) % 2 == 1 || !canMove )
		return;
	
	if (!valid_mv(x1, y1, x2, y2))
		return;
	
	
	tg.innerHTML = od.innerHTML;
	od.innerHTML = "";
	
	if (!valid()) {
		setTable(oldTable);
	} else {
		canMove = false;
		document.getElementById("load").style.display = "inline-block";
		opponent(oldTable);
	}
}

function drag(ev) {
  dragEl = ev.target;
}

function slot(x, y) {
	if (x % 2 == y % 2) {
		var x = document.getElementsByClassName("white")[Math.floor((x * 8 + y) / 2)];
	} else {
		var x = document.getElementsByClassName("black")[Math.floor((x * 8 + y) / 2)];
	}

	if (x.childNodes.length == 0)
		return Free;
	else {
		var t = x.childNodes[0].getAttribute("src").replace(".png", "");
		var c = ( t[0] == 'b' );
		var x = t.replace("b_", "").replace("w_", "");
		var p;

		if (x == "pawn")
			p = Pawn;
		else if (x == "rook")
			p = Rook;
		else if (x == "bishop")
			p = Bishop;
		else if (x == "knight")
			p = Knight;
		else if (x == "queen")
			p = Queen;
		else if (x == "king")
			p = King;
		
		return p * 2 + c;
	}
}

function slot_el(x, y) {
	if (x % 2 == y % 2) {
		var x = document.getElementsByClassName("white")[Math.floor((x * 8 + y) / 2)];
	} else {
		var x = document.getElementsByClassName("black")[Math.floor((x * 8 + y) / 2)];
	}

	return x;
}
</script>
</head>
<body style="-moz-user-select: none; -webkit-user-select: none; -ms-user-select:none; user-select:none;-o-user-select:none;" unselectable="on" onload="tryRestore()">
<div class="chessboard">
<!-- 1st -->
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_rook.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_knight.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_bishop.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_queen.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_king.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_bishop.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_knight.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_rook.png" draggable="true" ondragstart="drag(event)"></div>
<!-- 2nd -->
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="b_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<!-- 3th -->
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<!-- 4st -->
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<!-- 5th -->
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<!-- 6th -->
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"></div>
<!-- 7th -->
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_pawn.png" draggable="true" ondragstart="drag(event)"></div>
<!-- 8th -->
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_rook.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_knight.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_bishop.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_queen.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_king.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_bishop.png" draggable="true" ondragstart="drag(event)"></div>
<div class="black" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_knight.png" draggable="true" ondragstart="drag(event)"></div>
<div class="white" ondrop="drop(event)" ondragover="allowDrop(event)"><img src="w_rook.png" draggable="true" ondragstart="drag(event)"></div>
</div>
<img src="loading.gif" id="load" style="display: none;">
<pre>This is a smart chess player written by me. Have fun tring to beat him!
<b>Check out he Github repo! <a href="https://github.com/Tiberiu02/chess-meister/">Click!</a></pre>
<pre>Special credits go <a href="http://code2care.org/">code2care.org</a> the awesome chessboard!</pre>
</body>
</html>
