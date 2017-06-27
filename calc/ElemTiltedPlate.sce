n = 1.5 // IOR
a = 15 * %pi / 180 // incident angle
b = asin(sin(a)/n) // angle inside medium
L = 0.0045 // length of plate
L_axis = L / cos(b) // axial length of plate
L1 = L_axis * 0.2
L2 = L_axis - L1

////////////////////////////////////////////////////////////////////////
// precalculated full matrix of plate
s = n^2 - sin(a)^2
M_t_full = [1, L * n^2 * (1 - sin(a)^2) / sqrt(s^3); 0, 1]
M_s_full = [1, L                        / sqrt(s);   0, 1]
disp(M_t_full, "M_t(pre)")
disp(M_s_full, "M_s(pre)")

////////////////////////////////////////////////////////////////////////
// matrix of plate contained of 4 matrices 

// input into medium
M_in_t = [cos(b)/cos(a), 0; 0, 1/n * cos(a)/cos(b)]
M_in_s = [1, 0; 0, 1/n]
// left half-pass throught medium
M_1 = [1, L1; 0, 1] 
// right half-pass throught medium
M_2 = [1, L2; 0, 1] 
// output into air
M_out_t = [cos(a)/cos(b), 0; 0, n * cos(b)/cos(a)]
M_out_s = [1, 0; 0, n]

// matrix of plate
M_t_4 = M_out_t * M_2 * M_1 * M_in_t
M_s_4 = M_out_s * M_2 * M_1 * M_in_s
disp(M_t_4, "M_t(4)")
disp(M_s_4, "M_s(4)")

////////////////////////////////////////////////////////////////////////
// matrix of plate contained of 2 matrices 

// input into medium and left half-pass
M_lf_t = [cos(b)/cos(a), L1/n * cos(a)/cos(b); 0, 1/n * cos(a)/cos(b)]
M_lf_s = [1, L1/n; 0, 1/n]
// right half-pass and output into air
M_rt_t = [cos(a)/cos(b), L2 * cos(a)/cos(b); 0 n * cos(b)/cos(a)]
M_rt_s = [1, L2; 0, n]

M_t_2 = M_rt_t * M_lf_t
M_s_2 = M_rt_s * M_lf_s
disp(M_t_2, "M_t(2)") 
disp(M_s_2, "M_s(2)")
