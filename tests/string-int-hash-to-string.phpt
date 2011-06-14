--TEST--
Basic test for returning hashes as strings.
--INI--
xdebug.default_enable=0
--FILE--
<?php
$file = dirname( __FILE__ ) . "/simple.hash.string";
$hash = QuickHashStringIntHash::loadFromFile( $file );
$string = $hash->saveToString();

for ( $i = 0; $i < strlen( $string ); $i++ )
{
	printf( '%02X ', ord( $string[$i] ) );
	if ( $i % 16 == 15 ) {
		echo "\n";
	}
}
echo "\n\n";

$hash = QuickHashStringIntHash::loadFromString( $string );

foreach( range( 0, 0x1f ) as $key )
{
    $i = 48712 + $key * 1631;
    $k = base_convert( $i, 10, 36 );
    echo $k, ' => ', $hash->get( $k ), "\n";
}
?>
--EXPECT--
51 48 21 00 20 00 00 00 A0 00 00 00 40 00 00 00 
31 31 6C 34 00 31 32 75 66 00 31 34 33 71 00 31 
35 64 31 00 31 36 6D 63 00 31 37 76 6E 00 31 39 
34 79 00 31 61 65 39 00 31 62 6E 6B 00 31 63 77 
76 00 31 65 36 36 00 31 66 66 68 00 31 67 6F 73 
00 31 68 79 33 00 31 6A 37 65 00 31 6B 67 70 00 
31 6C 71 30 00 31 6D 7A 62 00 31 6F 38 6D 00 31 
70 68 78 00 31 71 72 38 00 31 73 30 6A 00 31 74 
39 75 00 31 75 6A 35 00 31 76 73 67 00 31 78 31 
72 00 31 79 62 32 00 31 7A 6B 64 00 32 30 74 6F 
00 32 32 32 7A 00 32 33 63 61 00 32 34 6C 6C 00 
03 00 00 00 03 00 00 00 05 00 00 00 A7 C4 00 00 
50 00 00 00 38 24 01 00 9B 00 00 00 C9 83 01 00 
06 00 00 00 02 00 00 00 2D 00 00 00 9F F7 00 00 
78 00 00 00 30 57 01 00 0A 00 00 00 03 00 00 00 
37 00 00 00 5D 04 01 00 41 00 00 00 1B 11 01 00 
8C 00 00 00 AC 70 01 00 0E 00 00 00 02 00 00 00 
0A 00 00 00 06 CB 00 00 96 00 00 00 6A 7D 01 00 
11 00 00 00 02 00 00 00 28 00 00 00 40 F1 00 00 
7D 00 00 00 8F 5D 01 00 15 00 00 00 01 00 00 00 
91 00 00 00 0B 77 01 00 18 00 00 00 01 00 00 00 
6E 00 00 00 72 4A 01 00 1F 00 00 00 01 00 00 00 
55 00 00 00 97 2A 01 00 20 00 00 00 01 00 00 00 
0F 00 00 00 65 D1 00 00 23 00 00 00 02 00 00 00 
69 00 00 00 13 44 01 00 82 00 00 00 EE 63 01 00 
26 00 00 00 01 00 00 00 5F 00 00 00 55 37 01 00 
27 00 00 00 02 00 00 00 00 00 00 00 48 BE 00 00 
32 00 00 00 FE FD 00 00 2A 00 00 00 02 00 00 00 
5A 00 00 00 F6 30 01 00 73 00 00 00 D1 50 01 00 
31 00 00 00 02 00 00 00 19 00 00 00 23 DE 00 00 
64 00 00 00 B4 3D 01 00 35 00 00 00 01 00 00 00 
23 00 00 00 E1 EA 00 00 38 00 00 00 01 00 00 00 
4B 00 00 00 D9 1D 01 00 3C 00 00 00 03 00 00 00 
14 00 00 00 C4 D7 00 00 1E 00 00 00 82 E4 00 00 
46 00 00 00 7A 17 01 00 3F 00 00 00 02 00 00 00 
3C 00 00 00 BC 0A 01 00 87 00 00 00 4D 6A 01 00 


11l4 => 48712
12uf => 50343
143q => 51974
15d1 => 53605
16mc => 55236
17vn => 56867
194y => 58498
1ae9 => 60129
1bnk => 61760
1cwv => 63391
1e66 => 65022
1ffh => 66653
1gos => 68284
1hy3 => 69915
1j7e => 71546
1kgp => 73177
1lq0 => 74808
1mzb => 76439
1o8m => 78070
1phx => 79701
1qr8 => 81332
1s0j => 82963
1t9u => 84594
1uj5 => 86225
1vsg => 87856
1x1r => 89487
1yb2 => 91118
1zkd => 92749
20to => 94380
222z => 96011
23ca => 97642
24ll => 99273