var CustomTerminal = function () { 

    this.terminal = new Terminal();
    this.terminal.open(document.getElementById('terminal_holder'));
    this.terminal.write('WoTServient$ ');


}

CustomTerminal.prototype.exec = function(command) {
    this.cmd = cp.exec(command, []);

    //handle stdout
    this.cmd.stdout.on('data', function(data) {
        console.log(data.toString())
        data.toString().split('\n').forEach(element => {
            this.terminal.writeln(element);
        });
    }.bind(this));

    //handle stderr
    this.cmd.stderr.on('data', function(data) {
        console.log(data.toString())
        data.toString().split('\n').forEach(element => {
            this.terminal.writeln('\x1B[1;3;31m'+element+'\x1B[0m');
        });
    }.bind(this));

    this.stdLine = '';
    //handle stdin
    this.terminal.onKey( (key, ev) => {
        this.stdLine += key.key;
        this.terminal.write(key.key);
        if (key.key.charCodeAt(0) == 13) {
            this.stdLine += '\n';
            this.cmd.stdin.write(this.stdLine);
            this.stdLine = '';
        }
        else if (key.key.charCodeAt(0) == 127) {
            this.terminal.write("\b \b");
            this.stdLine = this.stdLine.slice(0, -1);
        }
    })
    //on exit
    this.cmd.on('exit', (code) => {
        this.terminal.writeln('Child process exited with exit code '+code);
    });
}

// CustomTerminal.prototype.exec = function(command) {
//     this.cmd = cp.execFile(command, function (error, stdout, stderr) {
//         if (error) {
//           console.log(error.stack);
//           console.log('Error code: '+error.code);
//           console.log('Signal received: '+error.signal);
//         }
//         stdout = stdout.split('\n')
//         stdout.forEach(element => {
//             this.terminal.writeln(element)
//         });
//     }.bind(this));


// }
