open Express;
open Utils;
open Company;
open Employee;

let app = express();

[%%bs.raw{|
var bodyParser = require('body-parser');
app.use(bodyParser.json());
|}];

let companies: ref (list company) = ref [
	{ name: "SoftwareMill", employees: [
		{ firstName: "Daniel", lastName: "Kos", birthday: "1979-08-16", salary: 1000.0 },
		{ firstName: "Zenon", lastName: "Gamonski", birthday: "1943-06-16", salary: 1200.0 }		
	] },
	{ name: "UnsafeCode", employees: [
		{ firstName: "Jack", lastName: "Strong", birthday: "1987-01-15", salary: 5000.0 },
		{ firstName: "Jack", lastName: "Weak", birthday: "1983-05-10", salary: 6000.0 }		
	] }
];
let __dirname: option string = [%bs.node __dirname];
print_endline ("Current directory: " ^ (getOpt(__dirname, "-")));

module Response_ = {
	include Response;
	external setHeader: t => string => string => unit = "" [@@bs.send];
};

let employeeToJson = fun (e: employee) => {
	let json = Js_dict.empty();
	Js_dict.set json "firstName" (Js_json.string e.firstName);
	Js_dict.set json "lastName" (Js_json.string e.lastName);
	Js_dict.set json "birthday" (Js_json.string e.birthday);
	Js_dict.set json "salary" (Js_json.number e.salary);
	json;
};

let employeesToJson = fun (employees: list employee) => {
	Array.of_list employees 
	|> Array.map (fun e => Js_json.object_ (employeeToJson e));
};

let companyToJson = fun (c: company) => {
	let jsonEmployees = employeesToJson c.employees;
	let json = Js_dict.empty();
	Js_dict.set json "name" (Js_json.string c.name);
	Js_dict.set json "employees" (Js_json.array jsonEmployees);
	json;
};

let setRespHeaders = fun res => {
	Response_.setHeader res "Content-Type" "application/json";
	Response_.setHeader res "Access-Control-Allow-Origin" "*";
    Response_.setHeader res "Access-Control-Allow-Methods" "GET, POST, OPTIONS, PUT, PATCH, DELETE";
    Response_.setHeader res "Access-Control-Allow-Headers" "X-Requested-With,content-type";
};

exception CompanyNotFound;
exception NameFieldMissing;
exception QueryFieldMissing;
exception EmployeeFieldMissig;

let getBody = fun (body, key) => switch (body) {
	| Some json => getDictString json key
	| _ => None
	};

App.get app path::"/companies" @@ Middleware.from (fun _req res _next => {
	let jsonCompanies = !companies
		|> Array.of_list
		|> Array.map (fun c => Js.Json.string c.name);
	
	setRespHeaders res;
	Response_.sendJson res (Js_json.array jsonCompanies);
});

App.get app path::"/employees" @@ Middleware.from (fun req res _ => {
	let reqData = Request.query req;
	let name = getOptExc(getDictString reqData "name", NameFieldMissing);

	let cmps = !companies |> List.filter (fun c => c.name == name);

	if (List.length cmps == 0) {
		raise CompanyNotFound;
	};
	
	let company = List.hd cmps;
		
	let jsonEmployees = employeesToJson company.employees;

	setRespHeaders res;
	Response_.sendJson res (Js_json.array jsonEmployees);
});

App.get app path::"/search" @@ Middleware.from (fun req res _ => {
	let query = getOptExc(getDictString (Request.query req) "query", QueryFieldMissing);
	
	let names = !companies
		|> List.map (fun c => c.employees |> List.map (fun e => c.name ^ " " ^ e.firstName ^ " " ^ e.lastName))
		|> List.flatten;
	
	names |> List.iter (fun s => Js.log s);

	let jsonResults = names
		|> List.filter (fun name => contains (name, query))
		|> Array.of_list
		|> Array.map (fun name => Js.Json.string name);
	
	setRespHeaders res;
	Response_.sendJson res (Js_json.array jsonResults);
});

App.post app path::"/companies" @@ Middleware.from (fun req res _ => {
	let name = getDictString (Request.query req) "name";
	let defaultName = "Company " ^ string_of_int (List.length !companies + 1);
	companies := [{name: getOpt(name, defaultName), employees: []}] @ !companies;
	Response.sendJson res (makeSuccessJson())
});

App.post app path::"/employees" @@ Middleware.from (fun req res _ => {
	let reqData = Request.asJsonObject req;
	let body = getDictObject reqData "body";
	try {
		let name = getOptExc(getBody(body, "name"), CompanyNotFound);
		let firstName = getOptExc(getBody(body, "firstName"), EmployeeFieldMissig);
		let lastName = getOptExc(getBody(body, "lastName"), EmployeeFieldMissig);
		let birthday = getOptExc(getBody(body, "birthday"), EmployeeFieldMissig);
		let salary = getOpt(getBody(body, "salary"), "0.0");

		let company = List.find (fun c => c.name == name) !companies;
		companies := List.map(fun c => c.name == name ? {
			name: company.name,
			employees: company.employees @ [{
				firstName: firstName,
				lastName: lastName,
				birthday: birthday,
				salary: float_of_string salary
			}]
		} : c) !companies;
		Response.sendJson res (makeSuccessJson());
	} {
	| CompanyNotFound => Response.sendJson res (makeFailureJson "Company name was not found")
	| EmployeeFieldMissig => Response.sendJson res (makeFailureJson "One of required employee fields is missing")
	}
});
let statics = getOpt(__dirname, "/");
App.useOnPath app path::statics {
    let options = Static.defaultOptions ();
    Static.make "static" options |> Static.asMiddleware
};

App.listen app port::3000 ();
