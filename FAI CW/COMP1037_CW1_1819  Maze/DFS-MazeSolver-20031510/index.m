% This function returns the index of the location of a node in QUEUE
% Copyright 2009-2010 The MathWorks, Inc.

function n_index = index(QUEUE, xval, yval)
    i = 1;
    while(QUEUE(i, 2) ~= xval || QUEUE(i, 3) ~= yval )
        i = i + 1;
    end;
    n_index = i;
end