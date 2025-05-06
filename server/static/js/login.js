function login(event) {
  event.preventDefault();
  console.log(event.target);
  const formData = new FormData(event.target);
  fetch("/auth", {
    method: "POST",
    body: JSON.stringify({
      username: formData.get("username"),
      password: formData.get("password"),
    }),
  }).then((response) => {
    if (response.ok) {
      window.location.href = "map.html";
    } else {
      alert("Login failed. Please try again.");
    }
  });
}
