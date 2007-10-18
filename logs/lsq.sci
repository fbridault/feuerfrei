function [J,dJda,ind] = cost(a,ind);
x = [0.001 1 2 3 4 5 6]';
y = [100 25 5 2.5 1.5 1 0]';

vec=ones(7,1)
logax=log(a(2)*x)
yfit = 1-a(1)*logax
err = yfit-y;
J = err'*err;
dJda(1) = logax'*vec;
dJda(2) = a(1)/a(2);
endfunction

function [J,dJda,ind] = cost2(a,ind);
x = [0 1 3 7]';
y = [5 4 3 2]';
sinx = sin(a(2)*x);
yfit = a(1)*sinx;
err = yfit-y;
J = err'*err;
dJda(1) = -2*sinx'*err;
dJda(2) = -2*a(1)*(x.*cos(a(2)*x))'*err;
endfunction

function [J,dJda,ind] = cost3(a,ind);
x = [0.001 1 2 3 4 5 6]';
y = [100 25 5 2.5 1.5 1 0]';

vec=ones(7,1)
expax=exp(a(2)*x);
yfit = a(1)*expax;
err = yfit-y;
J = err'*err;
dJda(1) = expax'*vec;
dJda(2) = a(1)*a(2)*expax'*vec;
endfunction

function [c] = lin;
// On chercher à trouver une expression de la forme y=a.exp(bx)
// qui passe par les points suivants :
//x = [0.001 1 2 3 4 5 6];
//y = [100 25 5 2.5 1.5 1 0.01];

// Fluides
//y = [.60 .25 .05 .025 .015 .01 .001];
//x = [15 13 11 9 7 5 3];

// NURBS bougie
y = [1 .75 .4 .1 .03 .015];
x = [5 4 3 2 1 0];

// NURBS torche
//y = [1 .9 .5 .2 .05 .01];
//x = [5 4 3 2 1 0];

N=length(x);
// On définit une matrice de la forme [1 1;x x]
A=[ones(N,1) x(:)];
// Cette expression n'est pas une combinaison linéaire de a et b mais on peut faire
// le changement de variable z=log(y)=log(a)+bx
z=log(y)';
c=A \ z;

// On fait le changement inverse
yfit=exp(A*c);
c(1)=exp(c(1))
xbasc;

//plot2d(x, [y' yfit], style=[-6 1]);
// Les coefficients sont exp(c(1)) et c(2)
tmp=c(1)
c(1)=1/c(2)
c(2)=1/tmp
res=c(1)*log(y*c(2))
disp(res)
disp(c)
plot2d(y, [x' res'], style=[-6 5]);
endfunction
