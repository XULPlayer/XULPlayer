var browserObject = null;

function liteBrowserInit(homePage)
{
	browserObject = document.getElementById("mcex");
	if (homePage) {
		browserObject.homePage = homePage;
		browserObject.loadURI(homePage);
	}

}function browseHome()
{
	browserObject.goHome();
}

function browseNavigate(url)
{
	browserObject.loadURI(url);
}

function browseBack()
{
	if(browserObject.canGoBack) {
		browserObject.goBack();
	}
}

function browsePrev()
{
	if(browserObject.canGoForward) {
		browserObject.goForward();
	}
}

function browseRefresh()
{
	browserObject.reload();
}

function browseStop()
{
	browserObject.stop();
}

function browserGetCurrentUrl()
{
	return browserObject.currentURI.spec;
}