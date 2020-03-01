% Function to Populate QUEUE: generates a new node for QUEUE
% Copyright 2009-2010 The MathWorks, Inc.

function new_row = insert(xval, yval, parent_xval, parent_yval, gn, hn, fn)
    new_row = [1, 8];
    new_row(1, 1) = 1;
    new_row(1, 2) = xval;
    new_row(1, 3) = yval;
    new_row(1, 4) = parent_xval;
    new_row(1, 5) = parent_yval;
    new_row(1, 6) = gn;
    new_row(1, 7) = hn;
    new_row(1, 8) = fn;
end