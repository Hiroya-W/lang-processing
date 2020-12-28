program parseformalparameters;
procedure func(a,b : integer; c,d : char);
begin
    a := b;
    c := d;
end;
var x1,x2 : integer;
var y1,y2 : char;
begin
    call func(x1,x2,y1,y2);
end.
