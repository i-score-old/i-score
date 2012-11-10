// pattrstorage2minuit.js
// Interface entre pattrstorage et le protocole Minuit
// Karim Barkati - CICM
// Projet ANR Virage - Fevrier 2010
// v.20100223

inlets = 1;
outlets = 2;

// my Globals

var mynamespace = "MinuitDevice1";
var mynodes = new Array();
var myleaves = new Array();
var out_minuit = 0;
var out_ps = 1;


function from_minuit(chaine)
{
    var leavesRegExp = new RegExp(mynamespace + "/");

//    if (chaine.match(/^\?namespace/)) { 
    if (chaine.match(/^Virage\?namespace/)) { 
        set_namespace(chaine.replace(/^Virage\?namespace /, ""));
    }
//    else if (chaine.match(/^\?get/)) { 
    else if (chaine.match(/^Virage\?get/)) { 

        set_values(chaine.replace(/^Virage\?get /, ""));
    }
    else if (chaine.match(leavesRegExp)) { 
        to_pattrstorage(chaine.replace(leavesRegExp, ""));
    }
}

function from_pattrstorage(chaine)
{
    if (chaine.match(/^clientlist /)) { 
        clientlist(chaine.replace(/^clientlist /, ""));
    }
    else { // data reception
//        var reponse = ":get /" + mynamespace + "/" + chaine;
        var reponse = "MinuitDevice1:get /" + chaine;
        outlet(out_minuit, reponse);
    }
}

function to_pattrstorage(chaine)
{
    outlet(out_ps, chaine);
}

function set_namespace(chaine)
{
    var rootRegExp = new RegExp("^/" + mynamespace + "$");
    var leavesRegExp = new RegExp("^/");
//    var leavesRegExp = new RegExp("^/" + mynamespace + "/");

    if (chaine.match(/^\/$/)) {
//        outlet(out_minuit, mynamespace + ":namespace / nodes={ " + " }");
//    }
//    else if ( chaine.match(rootRegExp) )
//    {
        //mynodes = [];
        myleaves = [];
        outlet(out_ps, "getclientlist");
    }
    else if ( chaine.match(leavesRegExp) ) // leaves
    {
        outlet(out_minuit, mynamespace + ":namespace " + chaine + " attributes={ value }");
    }
}

function set_values(chaine)
{
//    var leavesRegExp = new RegExp("^/" + mynamespace + "/");
//    post("set_values(chaine) : " + chaine);
    var leavesRegExp = new RegExp("^/");

    if ( chaine.match(leavesRegExp) ) // leaves
    {
        var s = chaine.replace(leavesRegExp, "");
        s = chaine.replace("/", "");
        s = chaine.replace(":value", "");
//        post("s = " + s);
        outlet(out_ps, "get" + s );
    }
}

function clientlist(param)
{
    if (param == "done") { // ignoring "done" + dump pattrstorage
        //var nodes_answer = ":namespace /" + mynamespace + " nodes={ " + mynodes + " }";
//        var leaves_answer = ":namespace /" + mynamespace + " leaves={ " + myleaves + " }";
        var leaves_answer = mynamespace + ":namespace /" + " leaves={ " + myleaves + " }";
        outlet(out_minuit, leaves_answer.replace(/\,/g, " "));
        //outlet(out_minuit, nodes_answer.replace(/\,/g, " "));
    }
    else {
        myleaves.push(param);
    }
}

function top_node_name(chaine)
{
    mynamespace = chaine;
}