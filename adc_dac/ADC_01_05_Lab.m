
clear; clc; close all;

% ADC-01 to ADC-05: ideal conversion experiments
Vref = 3.3;
Vmin = 0;
resolutions = [8 10 12 16];

adc = @(v,n) min(2^n-1, max(0, ...
    floor((min(Vref,max(Vmin,v))-Vmin) ./ ...
    (Vref-Vmin) .* 2^n)));

reconstruct = @(code,n) Vmin + ...
    (double(code)+0.5) .* (Vref-Vmin) ./ 2^n;

dac = @(code,n) Vref .* min(2^n-1,max(0,double(code))) ...
    ./ (2^n-1);

%% ADC-01: Voltage to code
vin = 1.65;
code = adc(vin,12);

fprintf('\nADC-01: CONVERSION\n');
fprintf('Input voltage: %.4f V\n',vin);
fprintf('12-bit ADC code: %d\n',code);
fprintf('Reconstructed bin centre: %.6f V\n', ...
    reconstruct(code,12));

%% ADC-02: Quantization
t = linspace(0,1,4000);
analog = 1.65 + 1.4*sin(2*pi*3*t);
analog = min(Vref,max(Vmin,analog));

bits = 8;
codes = adc(analog,bits);
rec = reconstruct(codes,bits);
err = analog-rec;
LSB = (Vref-Vmin)/2^bits;

fprintf('\nADC-02: QUANTIZATION\n');
fprintf('LSB: %.4f mV\n',LSB*1000);
fprintf('Maximum sampled error: %.4f mV\n', ...
    max(abs(err))*1000);
fprintf('Ideal half-step bound: %.4f mV\n',LSB*500);

figure;
plot(t,analog,'LineWidth',1.2); hold on;
stairs(t,rec,'LineWidth',1.2);
xlabel('Time (s)'); ylabel('Voltage (V)');
title('ADC-02: Quantization Staircase');
legend('Analog input','Reconstructed value');
grid on;

figure;
plot(t,err*1000);
xlabel('Time (s)'); ylabel('Error (mV)');
title('ADC-02: Quantization Error');
grid on;

%% ADC-03: Resolution comparison
fprintf('\nADC-03: RESOLUTION COMPARISON\n');
for n = resolutions
    step = (Vref-Vmin)/2^n;
    fprintf('%2d-bit: %6d codes, LSB = %.6f mV\n', ...
        n,2^n,step*1000);
end

v = linspace(Vmin,Vref,1000);
figure; hold on;
for n = resolutions
    q = adc(v,n);
    plot(v,reconstruct(q,n),'DisplayName', ...
        sprintf('%d-bit',n));
end
xlabel('Input voltage (V)');
ylabel('Reconstructed voltage (V)');
title('ADC-03: Resolution Comparison');
legend('Location','northwest'); grid on;

%% ADC-04: Sampling rate and aliasing
fSignal = 50;
duration = 0.2;
tFine = linspace(0,duration,10000);
xFine = sin(2*pi*fSignal*tFine);

fsList = [30 80 200];

figure;
plot(tFine,xFine,'DisplayName','Original 50 Hz');
hold on;
for fs = fsList
    ts = 0:1/fs:duration;
    xs = sin(2*pi*fSignal*ts);
    plot(ts,xs,'o-','DisplayName',sprintf('%d Hz',fs));
    fprintf('fs=%d Hz; Nyquist=%.1f Hz; criterion=%d\n', ...
        fs,fs/2,fs>2*fSignal);
end
xlabel('Time (s)'); ylabel('Amplitude');
title('ADC-04: Sampling Rate Experiment');
legend; grid on;

%% ADC-05: DAC sample generation and reconstruction
fs = 1000;
duration = 0.1;
fTone = 50;
nBits = 8;

ts = 0:1/fs:duration-1/fs;
x = 1.65 + 1.4*sin(2*pi*fTone*ts);
x = min(Vref,max(Vmin,x));

dacCodes = adc(x,nBits);
dacLevels = dac(dacCodes,nBits);

tZOH = linspace(0,duration,10000);
index = floor(tZOH*fs)+1;
index = min(length(dacLevels),max(1,index));
zoh = dacLevels(index);

% Moving-average demonstration only, not a designed filter.
windowLength = 11;
filtered = conv(zoh,ones(1,windowLength)/windowLength,'same');

figure;
plot(tZOH,1.65+1.4*sin(2*pi*fTone*tZOH), ...
    'DisplayName','Reference sine');
hold on;
stairs(tZOH,zoh,'DisplayName','DAC zero-order hold');
plot(tZOH,filtered,'DisplayName','Moving-average demonstration');
xlabel('Time (s)'); ylabel('Voltage (V)');
title('ADC-05: DAC Waveform');
legend; grid on;

fprintf('\nADC-05: %d samples, %d-bit DAC model, fs=%.0f Hz\n', ...
    length(dacCodes),nBits,fs);
