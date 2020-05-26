var CustomTerminal = function () { 


    this.terminal = new Terminal();
    this.terminal.open(document.getElementById('terminal_holder'));
    this.terminal.write('Hello from \x1B[1;3;31mxterm.js\x1B[0m $ ');
    this.terminal.writeln('ll');
    
    this.terminal.onKey( (key, ev) => {
        console.log(key.key.charCodeAt(0));
        if (key.key.charCodeAt(0) == 13)
            this.terminal.write('\n');
        this.terminal.write(key.key);
    })
    
    const ls = exec('ls -l', function (error, stdout, stderr) {
        if (error) {
          console.log(error.stack);
          console.log('Error code: '+error.code);
          console.log('Signal received: '+error.signal);
        }
        stdout = stdout.split('\n')
        stdout.forEach(element => {
            this.terminal.writeln(element)
        });
    }.bind(this));

    ls.on('exit', function (code) {
        console.log('Child process exited with exit code '+code);
    });
}
