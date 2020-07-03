/**
 * On start, set flash checkbox and available device ports
 */
$(document).ready(function() {
    //set flash checkbox unchecked
    $('#flash').prop('checked', true);
    //get ports list
    var command = 'arduino-cli board list';
    result = cp.exec(command, []);
    //manage ports obtained
    result.stdout.on('data', function(data) {
        data.toString().split('\n').forEach(element => {
            if (element.includes("/dev/")) {
                var port = element.split(' ')[0];
                $("#serial_port").append(new Option(port, port));
            }
        });
    }.bind(this));

});

/**
 * Action triggered when submit-button is clicked
 */
$('#build_compile_button').on('click', function() {
    build_compile_flash();
});

/**
 * Action trtiggered when hide-show button is pressed.
 */
$("#hide_show").on('click', function() {
    hide_show_terminal();
});

$("#flash").change(function() {
    if ($(this).is(":checked")) {
        $("#serial_port").prop("disabled", false);
    } else {
        $("#serial_port").prop("disabled", true);
    }
});

/**
 * Hides terminal if shown and shows the terminal if hidden
 * @param {boolean} force if true forces opening 
 */
var hide_show_terminal = function(force) {
    if ($('#terminal_holder').is(":hidden") || force) {
        //show terminal
        $('#terminal_holder').show();
        $('#content').css("bottom", "300px");
        $('#footer').css("height", "300px");
    }
    else {
        $('#terminal_holder').hide();
        $('#content').css("bottom", "30px");
        $('#footer').css("height", "30px");
    }
}