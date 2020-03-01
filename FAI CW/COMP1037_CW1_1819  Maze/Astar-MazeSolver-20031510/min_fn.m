% Function to return the index of the node with minimum f(n) in QUEUE
% Copyright 2009-2010 The MathWorks, Inc.

function i_min = min_fn(QUEUE, QUEUE_COUNT)
 k = 1;
 temp_array = [];
 for j = 1 : QUEUE_COUNT
     if (QUEUE(j, 1) == 1)
         temp_array(k, :) = [QUEUE(j, :) j];
         k = k + 1;
     end;
 end; % get all nodes that are on QUEUE
 
 if size(temp_array ~= 0)
     [min_fn, temp_min] = min(temp_array(:, 8)); % index of the best node in temp array
     i_min = temp_array(temp_min, 9); % return its index in QUEUE
 else
     i_min = -1; % empty i.e no more paths are available.
 end;