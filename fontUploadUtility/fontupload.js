/**

==========================================================================
Font Upload Tool
==========================================================================
*/

/**

Configuration
*/

var FONT_FILE = "bitsyosd-font.mcm",
SERIAL_BAUD = 19200,
SERIAL_DEVICE = null;

/**

Objects
*/

var SerialPort = require("serialport"),
fs = require("fs"),
Prompt = require("readline-sync"),
ProgressBar = require("progress"),
Runtime = {
uploading: false
};

/**

(run)
*/

var run = function() {
try {

    if(SERIAL_DEVICE == null) {
        console.log("No valid COM Port selected.");
    } else {

        // initialize port
        var serialPort = new SerialPort(
            SERIAL_DEVICE,
            {
                baudRate: SERIAL_BAUD
            }
        );

        // open serial port
        serialPort.on("open", function () {

            // load
            fs.readFile(FONT_FILE, "utf-8", function(err, fileData) {

                // error handling
                if(err) {
                    throw err;
                }

                // transform data
                var d = fileData.split(/\r?\n/);

                // initialize bar
                var bar = new ProgressBar(
                    ":bar :current :percent",
                    {
                        total: d.length - 1
                    }
                );

                var pos = 1; // first position

                var next = function() {

                    if(pos >= d.length) {
                        return;
                    }

                    serialPort.write(d[pos] + "\r");
                    pos++;
                };


                // serial port
                serialPort.on("data", function(buffer) {

                    // prepare data
                    var data = ("" + buffer).trim().substr(0, 1);

                    // wait for start marker
                    switch(true) {

                        case data.toLowerCase() == "r":
                            console.log("Restarting font upload");
                            pos = 1;
                            next();
                            break;


                        case data.toLowerCase() == "d":
                            console.log(
                                "Font was successfully uploaded. All done."
                            );

                            // terminate application
                            process.exit();
                            break;


                        case data == "+":
                            if(!bar.complete) {
                                bar.tick();
                            }

                            next();
                            break;
                    }

                });

                // font upload starter
                setTimeout(function() {
                    next();
                }, 500);

            });

        });

        serialPort.on("error", function(err) {
            console.log("\nSerial Port Error: " + err.message + "\n");
        });
    }

} catch(e) {
    console.log("\nError: " + e + "\n");
}

};

/**

Select serial port
*/

if(!SERIAL_DEVICE) {

// display ports
SerialPort.list().then(function(ports) {

    if(!ports.length) {
        console.error("There are no serial ports detected");
        return;
    }

    ports.forEach(function(port, index) {
        console.log(index + " - " + port.path);
    });


    // create question
    var answer = parseInt(
        Prompt.question("\nEnter COM Port: ")
    );


    // set serial device
    SERIAL_DEVICE =
        answer >= 0 && answer < ports.length
            ? ports[answer].path
            : null;


    // run serial
    run();

}).catch(function(err) {
    console.error(
        "Unable to list serial ports: " + err.message
    );
});

} else {

// run serial
run();

}