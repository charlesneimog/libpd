const audioContextList = [];
    (function() {
        self.AudioContext = new Proxy(self.AudioContext, {
            construct(target, args) {
                const result = new target(...args);
                audioContextList.push(result);
                return result;
            }
        });
    });

function resumeAudio() {
    audioContextList.forEach(ctx => {
    if (ctx.state !== "running") { 
            ctx.resume(); 
        }
    });
}

const Module = { 
                preRun: [], 
                postRun: [], 
                print: function(e) {
                    1<arguments.length&&(e=Array.prototype.slice.call(arguments).join(" "));
                    console.log(e);
                }, 
                printErr: function(e) {
                    1<arguments.length&&(e=Array.prototype.slice.call(arguments).join(" "));
                    console.error(e)
                }
            };

var pdIsInitialized = false;

// ====================
Module.onRuntimeInitialized = () => {
    console.log("Pd is ready!");
    pdIsInitialized = true;
};

// ====================
function sendFloat(receiver, f) {
    if (!pdIsInitialized) {
        alert("Pd is not initialized yet!");
        return;
    }
    var str_raw = new TextEncoder().encode(receiver);
    var ptr = Module._webpd_malloc(str_raw.length + 1);
    var chunk = Module.HEAPU8.subarray(ptr, ptr + str_raw.length);
    chunk.set(str_raw);
    Module.HEAPU8[ptr + str_raw.length] = 0; // Null-terminate the string
    var result = Module._pd_sendFloat(ptr, f);
    Module._webpd_free(ptr);
    if (result !== 0) {
        console.error("Error sending float to pd");
    }
}

// ====================
function sendString(receiver, str){
    if (!pdIsInitialized) {
        alert("Pd is not initialized yet!");
        return;
    }
    var str_rawReceiver = new TextEncoder().encode(receiver);
    var ptrReceiver = Module._webpd_malloc(str_rawReceiver.length + 1);
    var chunkReceiver = Module.HEAPU8.subarray(ptrReceiver, ptrReceiver + str_rawReceiver.length);
    chunkReceiver.set(str_rawReceiver);
    Module.HEAPU8[ptrReceiver + str_rawReceiver.length] = 0; // Null-terminate the string

    var str_rawThing = new TextEncoder().encode(str);
    var ptrThing = Module._webpd_malloc(str_rawThing.length + 1);
    var chunkReceiver = Module.HEAPU8.subarray(ptrThing, ptrThing + str_rawThing.length);
    chunkReceiver.set(str_rawThing);
    Module.HEAPU8[ptrThing + str_rawThing.length] = 0; // Null-terminate the string

    var result = Module._pd_sendSymbol(ptrReceiver, ptrThing);

    Module._webpd_free(ptrReceiver);
    Module._webpd_free(ptrThing);
    
    if (result !== 0) {
        console.error("Error sending float to pd");
    }
}

