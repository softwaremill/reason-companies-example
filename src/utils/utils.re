external log: ('a) => unit = "console.log" [@@bs.val];

let makeSuccessJson () => {
  let json = Js.Dict.empty ();
  Js.Dict.set json "success" (Js.Json.boolean Js.true_);
  Js.Json.object_ json;
};

let getOpt (s: option 'a, dv: 'a) => (switch s { | Some(v) => v | _ => dv });

let getDictString dict key => {
	switch (Js.Dict.get dict key) {
	| Some json => Js.Json.decodeString json
	| _ => None
	};
};
let getDictNumber dict key => {
	switch (getDictString dict key) {
	| Some s => Some(float_of_string s)
	| _ => None
	};
};

let getDictObject dict key => {
	switch (Js.Dict.get dict key) {
	| Some json => Js.Json.decodeObject json
	| _ => None
	};
};
