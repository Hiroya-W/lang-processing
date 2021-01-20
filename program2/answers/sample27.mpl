program nwhilest;
var i, j, k : integer;
begin
    i := 1;
    while i < 10 do begin
        j := 1;
        while j < 10 do begin
            k := 1;
            while k < 10 do begin
                if (k div 2) * 2 = k then
                        k := k + 1
                    end
                else
                        k := k + 1
                    end
            end;
            j := j + 1;
        end;
        i := i + 1
    end;
    writeln('All End')
end.