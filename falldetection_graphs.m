
% --------------------- SETUP ---------------------

interv = 200;
limI = 0;
limS = interv;
passo = 1;
t = 1;

a.pinMode(2,'output');
a.pinMode(3,'input');
a.pinMode(8,'output'); 

Xoff = (289.7+377.1)/2;
Yoff = (302.5+388.3)/2;
Zoff = (280.2+366.9)/2;

sfact = 0;
fall = 0;
impact = 0;
queda = 0;

xvec = 0;
yvec = 0;
zvec = 0;
svec = 0;
tvec = 0;
fallvec = 0;
quedavec = 0;

i = 0;
j = 0;

% ------------------------------------------------

% --------------------- LOOP ---------------------

while(t<=limS)
    
    xout = 0;
    yout = 0;
    zout = 0;
    
    for i=0:2
        xout = xout + ((((a.analogRead(0) - Xoff)*4.97)/1023)/0.206);
        yout = yout + ((((a.analogRead(1) - Yoff)*4.97)/1023)/0.206);
        zout = zout + ((((a.analogRead(2) - Zoff)*4.97)/1023)/0.206);
        i = i+1;
    end
   
    xout = xout/3.0;
    yout = yout/3.0;
    zout = zout/3.0;
    
    sfact = sqrt(xout^2 + yout^2 + zout^2);
    
    if (sfact < 0.3) 
        fall = 1;
        j = 10;
    end    
    
    if (sfact > 2) 
        impact = 1;
        j = 10;
    end  
    
    if (fall == 1 & impact == 1 & j > 0)
        j = j - 1;
        queda = 1;
    elseif (j == 0)
        fall = 0;
        impact = 0;
        queda = 0;
    end
        
    t = t + passo;
    
    xvec = [xvec,xout];
    yvec = [yvec,yout];
    zvec = [zvec,zout];
    svec = [svec,sfact];
    tvec = [tvec,t];
    quedavec = [quedavec,queda];
    
    if (t > interv)      
        subplot(2,1,1);
        plot(tvec(t-interv:t),xvec(t-interv:t),tvec(t-interv:t),yvec(t-interv:t),tvec(t-interv:t),zvec(t-interv:t));
        axis([limI limS -2 3]);
        grid;
        
        subplot(2,1,2);
        plot(tvec(t-interv:t),quedavec(t-interv:t),tvec(t-interv:t),svec(t-interv:t));
        axis([limI limS 0 5]);
        grid;     
        
        limI = limI + 1;
        limS = limS + 1;
    else
        subplot(2,1,1);
        plot(tvec,xvec,tvec,yvec,tvec,zvec);
        axis([0 interv -2 3]);
        grid;
        
        subplot(2,1,2);
        plot(tvec,quedavec,tvec,svec);
        axis([0 interv 0 5]);
        grid;
    end;
    
    drawnow;
    
end

% ------------------------------------------------
