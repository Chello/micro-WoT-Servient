const fs = require('fs');
var term = {};

window.onload = function() {
    // var initial = {
    //     "title": "cosaccia",
    //     "id": "cosaccia",
    //     "@context": "https://www.w3.org/2019/wot/td/v1",
    //     "security": "nosec_sc",
    //     "securityDefinitions": {
    //         "nosec_sc": {
    //             "scheme": "nosec"
    //         }
    //     },
    //     "forms": [
    //         {
    //             "href": "",
    //             "contentType": "application/json",
    //             "op": [
    //                 "readallproperties",
    //                 "writeallproperties",
    //                 "readmultipleproperties",
    //                 "writemultipleproperties"
    //             ]
    //         }
    //     ],
    //     "properties": {
    //         "prop1": {
    //             "forms": [
    //                 {
    //                     "href": "",
    //                     "contentType": "application/json",
    //                     "op": [
    //                         "readproperty",
    //                         "writeproperty"
    //                     ]
    //                 },
    //                 {
    //                     "href": "",
    //                     "contentType": "application/json",
    //                     "op": [
    //                         "readproperty",
    //                         "writeproperty"
    //                     ]
    //                 }
    //             ],
    //             "type": "boolean",
    //             "observable": false,
    //             "readOnly": true,
    //             "writeOnly": true
    //         }
    //     },
    //     "actions": {
    //         "act1": {
    //             "forms": [
    //                 {
    //                     "href": "",
    //                     "contentType": "application/json",
    //                     "op": "invokeaction",
    //                     "httptermname": "httptermelem"
    //                 },
    //                 {
    //                     "href": "",
    //                     "contentType": "application/json",
    //                     "op": "invokeaction",
    //                     "wstermelem": "wstermeleeem"
    //                 }
    //             ],
    //             "input": {
    //                 "act1in1": {
    //                     "type": "boolean"
    //                 },
    //                 "act1in2": {
    //                     "type": "integer",
    //                     "minimum": 1,
    //                     "maximum": 10
    //                 }
    //             },
    //             "output": {
    //                 "type": "string"
    //             },
    //             "safe": true,
    //             "idempotent": false,
    //             "@type": [
    //                 "meta/type1",
    //                 "meta/type2"
    //             ],
    //             "title": "acttitle",
    //             "description": "actdesc",
    //             "term1name": "term1elem"
    //         },
    //         "act2": {
    //             "forms": [
    //                 {
    //                     "href": "",
    //                     "contentType": "application/json",
    //                     "op": "invokeaction"
    //                 }
    //             ],
    //             "input": {
    //                 "in": {
    //                     "type": "boolean"
    //                 }
    //             },
    //             "safe": false,
    //             "idempotent": false
    //         }
    //     }
    // };

    editor = new JSONEditor(document.getElementById('editor_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,

        //startval: initial,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/thing_desc.json",
            //$ref: "embeddedWoTServient/thing-schema.json"
        },
        //are fields all required? no
        required_by_default: false,
        //show checkbox for non-required opt
        show_opt_in: true,
        //show errors in editor
        show_errors: "change",
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });

    test = new JSONEditor(document.getElementById('test_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/test.json",
            //$ref: "embeddedWoTServient/thing-schema.json"
        },
        //are fields all required? no
        required_by_default: false,
        //show checkbox for non-required opt
        show_opt_in: true,
        //show errors in editor
        show_errors: "change",
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });

    builder = new JSONEditor(document.getElementById('build_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/build.json",
        },
        
        // Require all properties by default
        required_by_default: false,
        //show checkbox for non-required opt
        show_opt_in: true,
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });

    term = new CustomTerminal();
};
    
var composeTD = function() {
    //clone array
    //td = editor.getValue().map((x) => x);
    td = Object.assign({}, editor.getValue());
    options = Object.assign({}, builder.getValue());
    //properties arr becomes object of objects

    // if (td["properties"]) {
    //     td["properties"] = arrToObj(td["properties"], "propertyName");
    // }

    if (td["properties"]) {
        var arr = td['properties'];
        td['properties'] = {};
        arr.forEach(element => {
            var name = element.propertyName
            td['properties'][name] = element;
        });
    }

    if (td["actions"]) {
        var arr = td['actions'];
        td['actions'] = {};

        if (arr.length != 0) {
            options.actionFunctions = [];
        }

        arr.forEach(element => { //foreach action provided
            var name = element.actionName
            td['actions'][name] = element;

            //create array actionFunctions
            var actionCurrentFunction = {
                "source": 'cli',
                "name": name,
                "body": td['actions'][name]["body"],
                "output": {},
                "input": []
            };
            //if output is set
            if (td['actions'][name]['output']) {
                actionCurrentFunction.output = {
                    "name": '',
                    "type": td['actions'][name]['output']['type']
                }
            }
            //if inputs are set
            if (td['actions'][name]['input']) {
                var arr = td['actions'][name]['input'];
                td['actions'][name]['input'] = {};
                
                arr.forEach(element => { //foreach input provided
                    var inputName = element.inputName
                    td['actions'][name]['input'][inputName] = element;
                    //insert in options
                    actionCurrentFunction['input'].push({
                        "name": element.inputName,
                        "type": element.type
                    });
                });
            }

            options.actionFunctions.push(actionCurrentFunction);
        });
    }
    return [td, options];

}

// var arrToObj = function(arr, nameElement) {
//     // if (td["properties"]) {
//     //     var arr = td['properties'];
//     //     td['properties'] = {};
//     //     arr.forEach(element => {
//     //         var name = element.propertyName
//     //         td['properties'][name] = element;
//     //     });
//     // }
//     var obj = {};
//     arr.forEach(element => {
//         var 
//     })
// }


$('#submit_button').on('click', function() {

    console.log(editor.validate() /*? "true": "false"*/)
    console.log(builder.validate() /*? "true": "false"*/)

    // thing_prop = composeTD();
    // console.log(thing_prop)
    // build_prop = builder.getValue();
    var [thing_prop, build_prop] = composeTD();
    // console.log(editor.getValue());
    // console.log(builder.getValue());

    var thingName = thing_prop['title'].toLowerCase();
    console.log(thingName)

    //create dir
    fs.mkdir(path.join(__dirname, thingName), (err) => { 
        if (err) { 
            return console.error(err); 
        } 
        console.log('Directory created successfully!'); 
    }); 
    //generate file names
    var tdFile = path.join(__dirname, thingName + '/' + thingName + '.json');
    var optFile = path.join(__dirname, thingName + '/opt_' + thingName + '.json');
    var tmplFile =  $('#board').val()
    //write td file into dir
    fs.writeFile(tdFile, 
                JSON.stringify(thing_prop), 
                function(err) {
                    if (err) { 
                        return console.error(err); 
                    } 
                    console.log('File ${thingName}.json created successfully!'); 
                })
    //write builder option file into dir
    fs.writeFile(optFile, 
                JSON.stringify(build_prop), 
                function(err) {
                    if (err) { 
                        return console.error(err); 
                    } 
                    console.log('File ${thingName}.json created successfully!'); 
                })
    //generate exec string
    var exec_str =  'embeddedWoTServient/embeddedWoTServient.py build' +
                    ' -T ' + tdFile + 
                    ' -o ' + optFile +
                    ' -t ' + tmplFile;
    term.exec(exec_str)
});
