module Result = Relude.Result;
module Function = Relude.Function;

[@bs.module "@testing-library/react"]
external rawAct: (unit => Js.Undefined.t(Js.Promise.t('a))) => unit = "act";

[@bs.send.pipe: ReactTestingLibrary.renderResult]
external queryByTestId: string => Js.Null.t(Dom.element) = "queryByTestId";

[@bs.module "@testing-library/dom"]
external queryByText:
  (
    Dom.element,
    ~matcher: [@bs.unwrap] [
                | `Str(string)
                | `RegExp(Js.Re.t)
                | `Func((string, Dom.element) => bool)
              ],
    ~options: Js.undefined(DomTestingLibrary.Query.options)
  ) =>
  Js.Null.t(Dom.element) =
  "queryByText";

let act = callback =>
  rawAct(() => {
    callback();
    // Fix: Warning: The callback passed to act(...) function must return undefined, or a Promise.
    Js.Undefined.empty;
  });

let queryByText = (~matcher, ~options=?, result) =>
  queryByText(
    ~matcher,
    ~options=Js.Undefined.fromOption(options),
    result |> ReactTestingLibrary.container,
  );

module ApiMock = {
  open BsJestFetchMock;

  [@bs.scope "fetch"] [@bs.val]
  external fetch: {. "calls": array(array(string))} = "mock";

  module SampleData = {
    let articles = (~articlesCount=1, ()) =>
      Printf.sprintf(
        {|{
  "articles": [
    {
      "title": "How to train your dragon",
      "slug": "how-to-train-your-dragon-5w7g2y",
      "body": "Very carefully.",
      "createdAt": "2020-02-02T00:01:12.697Z",
      "updatedAt": "2020-02-02T00:01:12.697Z",
      "tagList": ["training", "dragons"],
      "description": "Ever wonder how?",
      "author": {
        "username": "jihchi",
        "bio": null,
        "image": "https://static.productionready.io/images/smiley-cyrus.jpg",
        "following": false
      },
      "favorited": false,
      "favoritesCount": 3
    }
  ],
  "articlesCount": %d
}|},
        articlesCount,
      );

    let feeds = (~articlesCount=1, ()) =>
      Printf.sprintf(
        {|{
  "articles": [
    {
      "title": "Would you like some sugar in your coffee?",
      "slug": "would-you-like-some-sugar-in-your-coffee-q9pur9",
      "body": "I don't want no sugar in my coffee. It'll make me mean.",
      "createdAt": "2019-03-14T12:56:00.783Z",
      "updatedAt": "2019-03-14T12:56:00.783Z",
      "tagList": ["coffee", "sugar"],
      "description":"No, thank you.",
      "author": {
        "username": "Clarabelle Kuhlman",
        "bio": "You can't program the feed without transmitting the back-end IB program!",
        "image": "https://s3.amazonaws.com/uifaces/faces/twitter/russoedu/128.jpg",
        "following": false
      },
      "favorited": false,
      "favoritesCount": 3
    }
  ],
  "articlesCount": %d
}|},
        articlesCount,
      );

    let tags = {|{
  "tags": [
    "butt",
    "dragons",
    "training",
    "coffee",
    "sushi"
  ]
}|};

    let user = {|{
  "user": {
    "id": 25902,
    "email": "achi@987.tw",
    "createdAt": "2018-04-09T15:16:52.642Z",
    "updatedAt": "2020-02-01T08:04:22.290Z",
    "username": "Jihchi Lee",
    "bio": null,
    "image": "",
    "token": "eyJ0eX.eyJpZCI.rLH25U9Z"
  }
}|};

    let article = {|{
  "article": {
    "title": "How to train your dragon",
    "slug": "how-to-train-your-dragon-sbr0z2",
    "body": "Very carefully.",
    "createdAt": "2018-06-03T15:44:01.786Z",
    "updatedAt": "2018-06-03T15:44:01.786Z",
    "tagList": ["training", "dragons"],
    "description": "Ever wonder how?",
    "author": {
      "username": "johnnyjacob",
      "bio": null,
      "image": "https://static.productionready.io/images/smiley-cyrus.jpg",
      "following": true
    },
    "favorited": true,
    "favoritesCount": 12
  }
}|};
  };

  let unathorized401 =
    JestFetchMock.init(~status=401, ~statusText="401 Unauthorized", ());

  let succeed: Result.t(string, string) => Result.t(string, string) =
    Result.flatMap(Result.pure);

  let pipe:
    (
      string => Result.t(string, string),
      Result.t(string, string) => Result.t(string, string),
      Result.t(string, string)
    ) =>
    Result.t(string, string) =
    fn => Function.map(Result.flatMap(fn));

  let articles:
    (
      ~articlesCount: int=?,
      Result.t(string, string) => Result.t(string, string),
      Result.t(string, string)
    ) =>
    Result.t(string, string) =
    (~articlesCount=1, bToC, a) =>
      pipe(
        pathname =>
          if (pathname == "/api/articles") {
            SampleData.articles(~articlesCount, ()) |> Result.error;
          } else {
            pathname |> Result.ok;
          },
        bToC,
        a,
      );

  let feeds:
    (
      ~articlesCount: int=?,
      Result.t(string, string) => Result.t(string, string),
      Result.t(string, string)
    ) =>
    Result.t(string, string) =
    (~articlesCount=1, bToC, a) =>
      pipe(
        pathname =>
          if (pathname == "/api/articles/feed") {
            SampleData.feeds(~articlesCount, ()) |> Result.error;
          } else {
            pathname |> Result.ok;
          },
        bToC,
        a,
      );

  let tags:
    (
      Result.t(string, string) => Result.t(string, string),
      Result.t(string, string)
    ) =>
    Result.t(string, string) =
    pipe(pathname =>
      if (pathname == "/api/tags") {
        SampleData.tags |> Result.error;
      } else {
        pathname |> Result.ok;
      }
    );

  let user:
    (
      Result.t(string, string) => Result.t(string, string),
      Result.t(string, string)
    ) =>
    Result.t(string, string) =
    pipe(pathname =>
      if (pathname == "/api/user") {
        SampleData.user |> Result.error;
      } else {
        pathname |> Result.ok;
      }
    );

  let article:
    (
      Result.t(string, string) => Result.t(string, string),
      Result.t(string, string)
    ) =>
    Result.t(string, string) =
    pipe(pathname =>
      if (pathname == "/api/articles/slug") {
        SampleData.article |> Result.error;
      } else {
        pathname |> Result.ok;
      }
    );

  let parseUrl: Fetch.Request.t => string =
    req => req |> Fetch.Request.url |> Webapi.Url.make |> Webapi.Url.pathname;

  let doMock = (~init=?, ~pipeline, ()) =>
    JestFetchMock.mockResponse(
      ~response=
        Fn(
          req => {
            let url: Result.t(string, string) =
              req |> parseUrl |> Result.pure;
            let result: Result.t(string, string) = url |> pipeline;

            result
            |> Result.flip
            |> Result.getOrElse("")
            |> Js.Promise.resolve;
          },
        ),
      ~init?,
      (),
    );
};

// this a work-around such that Jest won't emit warning
Jest.test("noop", () =>
  Jest.pass
);
