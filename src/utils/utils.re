[@bs.val] external log: 'a => unit = "console.log";

let makeSuccessJson = () => {
  let json = Js.Dict.empty ();
  Js.Dict.set(json, "success", Js.Json.boolean(Js.true_));
  Js.Json.object_(json);
};

let makeFailureJson = (msg: string) => {
  let json = Js.Dict.empty ();
  Js.Dict.set(json, "error", Js.Json.string(msg));
  Js.Json.object_(json);
};

let getOpt = (s: option('a), dv: 'a) => switch (s) { | Some(v) => v | _ => dv };

let getOptExc = (s: option('a), exc: 'b) => switch (s) { | Some(v) => v | _ => raise(exc) };

let getDictString = (dict, key) => {
	switch (Js.Dict.get(dict, key)) {
	| Some(json) => Js.Json.decodeString(json)
	| _ => None
	};
};
let getDictNumber = (dict, key) => {
	switch (getDictString(dict, key)) {
	| Some(s) => Some(float_of_string(s))
	| _ => None
	};
};

let getDictObject = (dict, key) => {
	switch (Js.Dict.get(dict, key)) {
	| Some(json) => Js.Json.decodeObject(json)
	| _ => None
	};
};

let str = ReasonReact.stringToElement;
let listToElement = (elements: list('a)) => ReasonReact.arrayToElement (Array.of_list(elements));

let contains = fun (s: string, q: string) => {
	let ls = String.length(s);
	let lq = String.length(q);
	if (lq > ls) {
		false;
	} else {
		let i = ref(0);
		let j = ref(0);
		while (i^ + j^ < ls && j^ < lq) {
			let sc = String.get(s, (i^ + j^));
			let qc = String.get(q, j^);

			if (sc == qc) {
				j := j^ + 1;
			} else {
				i := i^ + 1;
				j := 0;
			}
		};
		if (j^ == lq) {
			true;
		} else {
			false
		};
	}
};
